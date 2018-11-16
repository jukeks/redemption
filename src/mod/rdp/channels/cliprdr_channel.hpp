/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2015
    Author(s): Christophe Grosjean, Raphael Zhou
*/


#pragma once

#include "core/channel_list.hpp"
#include "core/front_api.hpp"
#include "core/RDP/clipboard.hpp"
#include "mod/rdp/channels/base_channel.hpp"
#include "mod/rdp/channels/sespro_launcher.hpp"
#include "system/linux/system/ssl_sha256.hpp"
#include "utils/key_qvalue_pairs.hpp"
#include "utils/sugar/algostring.hpp"
#include "utils/stream.hpp"
#include "utils/difftimeval.hpp"
#include "utils/uninit_checked.hpp"

#include <map>
#include <memory>
#include <vector>

#define FILE_LIST_FORMAT_NAME "FileGroupDescriptorW"


class ClipboardVirtualChannel final : public BaseVirtualChannel
{
private:
    using format_name_inventory_type = std::map<uint32_t, std::string>;
    format_name_inventory_type format_name_inventory;

    struct file_contents_request_info
    {
        uint32_t lindex;
        uint64_t position;
        uint32_t cbRequested;
        uint32_t clipDataId;
        uint32_t offset;
    };
    // TODO strong type for streamId
    using file_contents_request_info_inventory_type = std::map<uint32_t /*streamId*/, file_contents_request_info>;

    struct file_info_type
    {
        std::string file_name;
        uint64_t size;
        uint64_t sequential_access_offset;
        SslSha256 sha256;
    };
    using file_info_inventory_type = std::vector<file_info_type>;

    // TODO strong type for clipDataId
    using file_stream_data_inventory_type = std::map<uint32_t /*clipDataId*/, file_info_inventory_type>;

    struct ClipboardData
    {
        file_contents_request_info_inventory_type file_contents_request_info_inventory;
        file_stream_data_inventory_type file_stream_data_inventory;
        uint32_t clip_data_id = 0;
        uint32_t file_list_format_id = 0;
        uint32_t data_len = 0;
        uint32_t stream_id = 0;
        // TODO unused for client ?
        uint16_t message_type = 0;
        bool use_long_format_names = false;
    };

    ClipboardData client_data;
    ClipboardData server_data;

    uint32_t requestedFormatId = 0;

    const bool param_clipboard_down_authorized;
    const bool param_clipboard_up_authorized;
    const bool param_clipboard_file_authorized;

    const bool param_dont_log_data_into_syslog;
    const bool param_dont_log_data_into_wrm;

    const bool param_log_only_relevant_clipboard_activities;

    StaticOutStream<RDPECLIP::FileDescriptor::size()> file_descriptor_stream;

    FrontAPI& front;

    SessionProbeLauncher* clipboard_monitor_ready_notifier = nullptr;
    SessionProbeLauncher* clipboard_initialize_notifier    = nullptr;
    SessionProbeLauncher* format_list_notifier             = nullptr;
    SessionProbeLauncher* format_list_response_notifier    = nullptr;
    SessionProbeLauncher* format_data_request_notifier     = nullptr;

    const bool proxy_managed;   // Has not client.

public:
    struct Params : public BaseVirtualChannel::Params {
        uninit_checked<bool> clipboard_down_authorized;
        uninit_checked<bool> clipboard_up_authorized;
        uninit_checked<bool> clipboard_file_authorized;

        uninit_checked<bool> dont_log_data_into_syslog;
        uninit_checked<bool> dont_log_data_into_wrm;

        uninit_checked<bool> log_only_relevant_clipboard_activities;

        explicit Params(ReportMessageApi & report_message)
          : BaseVirtualChannel::Params(report_message)
        {}
    };

    ClipboardVirtualChannel(
        VirtualChannelDataSender* to_client_sender_,
        VirtualChannelDataSender* to_server_sender_,
        FrontAPI& front,
        const Params & params)
    : BaseVirtualChannel(to_client_sender_,
                         to_server_sender_,
                         params)
    , param_clipboard_down_authorized(params.clipboard_down_authorized)
    , param_clipboard_up_authorized(params.clipboard_up_authorized)
    , param_clipboard_file_authorized(params.clipboard_file_authorized)
    , param_dont_log_data_into_syslog(params.dont_log_data_into_syslog)
    , param_dont_log_data_into_wrm(params.dont_log_data_into_wrm)
    , param_log_only_relevant_clipboard_activities(params.log_only_relevant_clipboard_activities)

    , front(front)
    , proxy_managed(to_client_sender_ == nullptr) {
    }

protected:
    const char* get_reporting_reason_exchanged_data_limit_reached() const
        override
    {
        return "CLIPBOARD_LIMIT";
    }

public:
    bool use_long_format_names() const {
        return (this->client_data.use_long_format_names &&
            this->server_data.use_long_format_names);
    }

private:
    void process_clipboard_capabilities_pdu(
        InStream& chunk, ClipboardData& clipboard_data, char const* log_type)
    {
        const uint16_t cCapabilitiesSets = chunk.in_uint16_le();
        assert(1 == cCapabilitiesSets);

        chunk.in_skip_bytes(2); // pad1(2)

        for (uint16_t i = 0; i < cCapabilitiesSets; ++i) {
            RDPECLIP::CapabilitySetRecvFactory f(chunk);

            if (f.capabilitySetType() == RDPECLIP::CB_CAPSTYPE_GENERAL) {
                RDPECLIP::GeneralCapabilitySet general_caps;

                general_caps.recv(chunk, f);

                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_%s_clipboard_capabilities_pdu: "
                            "General Capability Set", log_type);
                    general_caps.log(LOG_INFO);
                }

                clipboard_data.use_long_format_names
                    = (general_caps.generalFlags() & RDPECLIP::CB_USE_LONG_FORMAT_NAMES);
            }
        }
    }

    bool process_client_clipboard_capabilities_pdu(InStream& chunk)
    {
        this->client_data.use_long_format_names = false;
        this->process_clipboard_capabilities_pdu(chunk, this->client_data, "client");
        return true;
    }

    bool process_client_file_contents_request(InStream& chunk, const RDPECLIP::CliprdrHeader & in_header)
    {
        RDPECLIP::FileContentsRequestPDU file_contents_request_pdu;

        if (in_header.dataLen() > RDPECLIP::FileContentsRequestPDU::minimum_size()) {
            file_contents_request_pdu.receive(chunk);
            if (bool(this->verbose & RDPVerbose::cliprdr)) {
                file_contents_request_pdu.log(LOG_INFO);
            }
        }

        this->set_file_contens_request_info(this->client_data, file_contents_request_pdu);

        if (!this->param_clipboard_file_authorized) {
            this->process_file_contents_request(
                this->to_client_sender_ptr(),
                file_contents_request_pdu.dwFlags(),
                file_contents_request_pdu.streamId(),
                "client");

            return false;
        }

        return true;
    }

    bool process_client_format_data_request_pdu(uint32_t total_length,
        uint32_t flags, InStream& chunk, const RDPECLIP::CliprdrHeader & /*in_header*/)
    {
        (void)total_length;
        (void)flags;

        {
            const unsigned int expected = 4;   //     requestedFormatId(4)
            if (!chunk.in_check_rem(expected)) {
                LOG(LOG_ERR,
                    "ClipboardVirtualChannel::process_client_format_data_request_pdu: "
                        "Truncated CLIPRDR_FORMAT_DATA_REQUEST, need=%u remains=%zu",
                    expected, chunk.in_remain());
                throw Error(ERR_RDP_DATA_TRUNCATED);
            }
        }

        this->requestedFormatId = chunk.in_uint32_le();

        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::process_client_format_data_request_pdu: "
                    "requestedFormatId=%s(%u)",
                RDPECLIP::get_FormatId_name(this->requestedFormatId),
                this->requestedFormatId);
        }

        if (!this->param_clipboard_down_authorized) {
            this->process_format_data_request_send(this->to_client_sender_ptr(), "client");

            return false;
        }

        return true;
    }

    bool process_client_format_data_response_pdu(uint32_t total_length,
        uint32_t flags, InStream& chunk, const RDPECLIP::CliprdrHeader & in_header)
    {
        (void)total_length;

        this->proccess_format_data_response_pdu(
            flags, chunk.clone(), in_header, this->client_data);

        return true;
    }   // process_client_format_data_response_pdu

private:
    void log_file_info(file_info_type & file_info, bool from_remote_session)
    {
        const char* type = (
                  from_remote_session
                ? "CB_COPYING_PASTING_FILE_FROM_REMOTE_SESSION"
                : "CB_COPYING_PASTING_FILE_TO_REMOTE_SESSION"
            );

        uint8_t digest[SslSha256::DIGEST_LENGTH] = { 0 };

        file_info.sha256.final(digest);

        char digest_s[128];
        snprintf(digest_s, sizeof(digest_s),
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            digest[ 0], digest[ 1], digest[ 2], digest[ 3], digest[ 4], digest[ 5], digest[ 6], digest[ 7],
            digest[ 8], digest[ 9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15],
            digest[16], digest[17], digest[18], digest[19], digest[20], digest[21], digest[22], digest[23],
            digest[24], digest[25], digest[26], digest[27], digest[28], digest[29], digest[30], digest[31]);

        auto const file_size_str = std::to_string(file_info.size);

        auto const info = key_qvalue_pairs({
                { "type", type },
                { "file_name", file_info.file_name },
                { "size", file_size_str },
                { "sha256", digest_s }
            });

        ArcsightLogInfo arc_info;
        arc_info.name = type;
        arc_info.ApplicationProtocol = "rdp";
        arc_info.fileName = file_info.file_name;
        arc_info.fileSize = file_info.size;
        arc_info.WallixBastionSHA256Digest = digest_s;
        arc_info.direction_flag = from_remote_session ? ArcsightLogInfo::SERVER_SRC : ArcsightLogInfo::SERVER_DST;

        this->report_message.log6(info, arc_info, tvtime());

        if (!this->param_dont_log_data_into_syslog) {
            LOG(LOG_INFO, "%s", info);
        }

        if (!this->param_dont_log_data_into_wrm) {
            std::string message(type);
            message += "=";
            message += file_info.file_name;
            message += "\x01";
            message += file_size_str;
            message += "\x01";
            message += digest_s;

            this->front.session_update(message);
        }
    }

public:
    bool process_client_format_list_pdu(uint32_t total_length, uint32_t flags,
        InStream& chunk, const RDPECLIP::CliprdrHeader & in_header)
    {
        (void)total_length;

        if (!this->param_clipboard_down_authorized &&
            !this->param_clipboard_up_authorized &&
            !this->format_list_response_notifier) {
            LOG(LOG_WARNING,
                "ClipboardVirtualChannel::process_client_format_list_pdu: "
                    "Clipboard is fully disabled.");

            this->process_format_list_response_disabled(this->to_client_sender_ptr());
            return false;
        }

        if (!(flags & CHANNELS::CHANNEL_FLAG_LAST)) {
            LOG(LOG_ERR,
                "ClipboardVirtualChannel::process_client_format_list_pdu: "
                    "!!!CHUNKED!!! Format List PDU is not yet supported!");

            this->process_format_list_response_disabled(this->to_client_sender_ptr());
            return false;
        }

        this->process_format_list_response(
            chunk, in_header, this->client_data, "client");

        return true;
    }

    static void log_client_message_type(uint16_t message_type, uint32_t flags)
    {
        const char * message_type_str([](uint16_t message_type){
            switch (message_type){
            case RDPECLIP::CB_CLIP_CAPS:
                return "Clipboard Capabilities PDU";
            case RDPECLIP::CB_FORMAT_LIST:
                return "Clipboard Format List PDU";
            case RDPECLIP::CB_FORMAT_DATA_REQUEST:
                return "Clipboard Format Data Request PDU";
            case RDPECLIP::CB_FILECONTENTS_REQUEST:
                return "Clipboard File Contents Request PDU";
            case RDPECLIP::CB_FORMAT_DATA_RESPONSE:
                return "Clipboard Format Data Response PDU";
            case RDPECLIP::CB_FILECONTENTS_RESPONSE:
                return "Clipboard File Contents Response PDU";
            case RDPECLIP::CB_LOCK_CLIPDATA:
                return "Lock Clipboard Data PDU";
            case RDPECLIP::CB_UNLOCK_CLIPDATA:
                return "Unlock Clipboard Data PDU";
            default:
                return RDPECLIP::get_msgType_name(message_type);
            }
        }(message_type));
        LOG(LOG_INFO, "ClipboardVirtualChannel::process_client_message: %s (%s:%s)",
            message_type_str,
            (flags & CHANNELS::CHANNEL_FLAG_FIRST) ? "FIRST" : "",
            (flags & CHANNELS::CHANNEL_FLAG_LAST) ? "LAST" : "");
    }


public:
    void process_client_message(uint32_t total_length,
        uint32_t flags, const uint8_t* chunk_data,
        uint32_t chunk_data_length) override
    {
        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::process_client_message: "
                    "total_length=%u flags=0x%08X chunk_data_length=%u",
                total_length, flags, chunk_data_length);
        }

        if (bool(this->verbose & RDPVerbose::cliprdr_dump)) {
            const bool send              = false;
            const bool from_or_to_client = true;
            ::msgdump_c(send, from_or_to_client, total_length, flags,
                chunk_data, chunk_data_length);
        }

        InStream chunk(chunk_data, chunk_data_length);
        RDPECLIP::CliprdrHeader header;

        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
            if (!chunk.in_check_rem(8 /* msgType(2) + msgFlags(2) + dataLen(4) */)) {
                LOG(LOG_ERR,
                    "ClipboardVirtualChannel::process_client_message: "
                        "Truncated msgType, need=2 remains=%zu",
                    chunk.in_remain());
                throw Error(ERR_RDP_DATA_TRUNCATED);
            }

            header.recv(chunk);

            this->client_data.message_type = header.msgType();
        }

        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            log_client_message_type(this->client_data.message_type, flags);
        }

        bool send_message_to_server = true;

        switch (this->client_data.message_type)
        {
            case RDPECLIP::CB_CLIP_CAPS:
                send_message_to_server =
                    this->process_client_clipboard_capabilities_pdu(chunk);
            break;

            case RDPECLIP::CB_FORMAT_LIST:
                send_message_to_server =
                    this->process_client_format_list_pdu(
                        total_length, flags, chunk, header);
            break;

            case RDPECLIP::CB_FORMAT_DATA_REQUEST:
                send_message_to_server =
                    this->process_client_format_data_request_pdu(
                        total_length, flags, chunk, header);
            break;

            case RDPECLIP::CB_FILECONTENTS_REQUEST:
                send_message_to_server = this->process_client_file_contents_request(chunk, header);
            break;

            case RDPECLIP::CB_FORMAT_DATA_RESPONSE:
                send_message_to_server =
                    this->process_client_format_data_response_pdu(
                        total_length, flags, chunk, header);

                if (send_message_to_server &&
                    (flags & CHANNELS::CHANNEL_FLAG_FIRST)) {
                    this->update_exchanged_data(total_length);
                }
            break;

            case RDPECLIP::CB_FILECONTENTS_RESPONSE: {
                this->process_message_filecontents_response(
                    total_length, flags, chunk, header,
                    this->client_data, this->server_data);
            }
            break;

            case RDPECLIP::CB_LOCK_CLIPDATA: {
                this->process_message_lock_clipdata(
                    chunk, header, this->server_data, "client");
            }
            break;

            case RDPECLIP::CB_UNLOCK_CLIPDATA: {
                this->process_message_unlock_clipdata(
                    chunk, header, this->server_data, "client");
            }
            break;

            default:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_client_message: "
                            "Delivering unprocessed messages %s(%u) to server.",
                        RDPECLIP::get_msgType_name(this->client_data.message_type),
                        static_cast<unsigned>(this->client_data.message_type));
                }
            break;
        }   // switch (this->client_data.message_type)

        if (send_message_to_server) {
            this->send_message_to_server(total_length, flags, chunk_data,
                chunk_data_length);
        }
    }   // process_client_message

    bool process_server_clipboard_capabilities_pdu(uint32_t total_length,
        uint32_t flags, InStream& chunk, const RDPECLIP::CliprdrHeader & /*in_header*/)
    {
        (void)total_length;
        (void)flags;

        this->process_clipboard_capabilities_pdu(chunk, this->server_data, "server");

        return !this->proxy_managed;
    }

    bool process_server_file_contents_request_pdu(uint32_t total_length,
        uint32_t flags, InStream& chunk, const RDPECLIP::CliprdrHeader & in_header)
    {
        (void)total_length;
        (void)flags;

        if (in_header.dataLen() > RDPECLIP::FileContentsRequestPDU::minimum_size()) {
            RDPECLIP::FileContentsRequestPDU file_contents_request_pdu;

            file_contents_request_pdu.receive(chunk);
            if (bool(this->verbose & RDPVerbose::cliprdr)) {
                file_contents_request_pdu.log(LOG_INFO);
            }

            if (!this->param_clipboard_file_authorized) {
                this->process_file_contents_request(
                    this->to_server_sender_ptr(),
                    file_contents_request_pdu.dwFlags(),
                    file_contents_request_pdu.streamId(),
                    "server");

                return false;
            }

            this->set_file_contens_request_info(this->server_data, file_contents_request_pdu);
        }

        return true;
    }

    bool process_server_format_data_request_pdu(uint32_t total_length,
        uint32_t flags, InStream& chunk, const RDPECLIP::CliprdrHeader & /*in_header*/)
    {
        (void)total_length;
        (void)flags;

        this->requestedFormatId = chunk.in_uint32_le();

        if (this->format_data_request_notifier &&
            (this->requestedFormatId == RDPECLIP::CF_TEXT)) {
            if (!this->format_data_request_notifier->on_server_format_data_request()) {
                this->format_data_request_notifier = nullptr;
            }

            return false;
        }

        if (!this->param_clipboard_up_authorized) {
            this->process_format_data_request_send(this->to_server_sender_ptr(), "server");

            return false;
        }

        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::process_server_format_data_request_pdu: "
                    "requestedFormatId=%s(%u)",
                RDPECLIP::get_FormatId_name(this->requestedFormatId),
                this->requestedFormatId);
        }

        return true;
    }   // process_server_format_data_request_pdu

    bool process_server_format_data_response_pdu(uint32_t total_length,
        uint32_t flags, InStream& chunk, const RDPECLIP::CliprdrHeader & in_header)
    {
        (void)total_length;

        this->proccess_format_data_response_pdu(
            flags, chunk.clone(), in_header, this->server_data);

        return true;
    }   // process_server_format_data_response_pdu

    bool process_server_format_list_pdu(uint32_t total_length, uint32_t flags,
        InStream& chunk, const RDPECLIP::CliprdrHeader & in_header)
    {
        (void)total_length;
        (void)flags;

        if (!this->param_clipboard_down_authorized &&
            !this->param_clipboard_up_authorized) {
            LOG(LOG_WARNING,
                "ClipboardVirtualChannel::process_server_format_list_pdu: "
                    "Clipboard is fully disabled.");

            this->process_format_list_response_disabled(this->to_server_sender_ptr());
            return false;
        }

        this->process_format_list_response(
            chunk, in_header, this->server_data, "server");

        return true;
    }   // process_server_format_list_pdu

    bool process_server_monitor_ready_pdu(uint32_t total_length, uint32_t flags,
        InStream& chunk, const RDPECLIP::CliprdrHeader & /*in_header*/)
    {
        (void)total_length;
        (void)flags;
        (void)chunk;

        if (this->proxy_managed) {
            // Client Clipboard Capabilities PDU.
            {
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_monitor_ready_pdu: "
                            "Send Clipboard Capabilities PDU.");
                }

                RDPECLIP::GeneralCapabilitySet general_cap_set(
                    RDPECLIP::CB_CAPS_VERSION_1,
                    RDPECLIP::CB_USE_LONG_FORMAT_NAMES);
                RDPECLIP::ClipboardCapabilitiesPDU clipboard_caps_pdu(1);
                RDPECLIP::CliprdrHeader clipboard_header(RDPECLIP::CB_CLIP_CAPS, 0,
                    clipboard_caps_pdu.size() + general_cap_set.size());

                StaticOutStream<1024> out_stream;

                clipboard_header.emit(out_stream);
                clipboard_caps_pdu.emit(out_stream);
                general_cap_set.emit(out_stream);

                const uint32_t total_length      = out_stream.get_offset();
                const uint32_t flags             =
                    CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST;
                const uint8_t* chunk_data        = out_stream.get_data();
                const uint32_t chunk_data_length = total_length;

                this->send_message_to_server(
                    total_length,
                    flags,
                    chunk_data,
                    chunk_data_length);
            }

            this->client_data.use_long_format_names = true;

            // Format List PDU.
            {
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_monitor_ready_pdu: "
                            "Send Format List PDU.");
                }

                RDPECLIP::FormatListPDUEx format_list_pdu;
                format_list_pdu.add_format_name(RDPECLIP::CF_TEXT);

                const bool use_long_format_names = this->use_long_format_names();
                const bool in_ASCII_8 = format_list_pdu.will_be_sent_in_ASCII_8(use_long_format_names);

                RDPECLIP::CliprdrHeader clipboard_header(RDPECLIP::CB_FORMAT_LIST,
                    RDPECLIP::CB_RESPONSE__NONE_ | (in_ASCII_8 ? RDPECLIP::CB_ASCII_NAMES : 0),
                    format_list_pdu.size(use_long_format_names));

                StaticOutStream<256> out_s;

                clipboard_header.emit(out_s);
                format_list_pdu.emit(out_s, use_long_format_names);

                const size_t totalLength = out_s.get_offset();
                InStream in_s(out_s.get_data(), totalLength);

                this->send_message_to_server(
                        totalLength,
                        CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST | CHANNELS::CHANNEL_FLAG_SHOW_PROTOCOL,
                        out_s.get_data(),
                        totalLength
                    );
            }

            return false;
        }

        if (this->clipboard_monitor_ready_notifier) {
            if (!this->clipboard_monitor_ready_notifier->on_clipboard_monitor_ready()) {
                this->clipboard_monitor_ready_notifier = nullptr;
            }
        }

        return true;
    }

    void process_server_message(uint32_t total_length,
        uint32_t flags, const uint8_t* chunk_data,
        uint32_t chunk_data_length,
        std::unique_ptr<AsynchronousTask> & out_asynchronous_task) override
    {
        (void)out_asynchronous_task;

        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::process_server_message: "
                    "total_length=%u flags=0x%08X chunk_data_length=%u",
                total_length, flags, chunk_data_length);
        }

        if (bool(this->verbose & RDPVerbose::cliprdr_dump)) {
            const bool send              = false;
            const bool from_or_to_client = false;
            ::msgdump_c(send, from_or_to_client, total_length, flags,
                chunk_data, chunk_data_length);
        }

        InStream chunk(chunk_data, chunk_data_length);
        RDPECLIP::CliprdrHeader header;

        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
            if (!chunk.in_check_rem(8 /* msgType(2) + msgFlags(2) + dataLen(4) */)) {
                LOG(LOG_ERR,
                    "ClipboardVirtualChannel::process_server_message: "
                        "Truncated msgType, need=2 remains=%zu",
                    chunk.in_remain());
                throw Error(ERR_RDP_DATA_TRUNCATED);
            }

            header.recv(chunk);

            this->server_data.message_type = header.msgType();
        }

        bool send_message_to_client = true;

        switch (this->server_data.message_type)
        {
            case RDPECLIP::CB_CLIP_CAPS:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Clipboard Capabilities PDU");
                }

                send_message_to_client =
                    this->process_server_clipboard_capabilities_pdu(
                        total_length, flags, chunk, header);
            break;

            case RDPECLIP::CB_FILECONTENTS_REQUEST:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "File Contents Request PDU");
                }

                send_message_to_client =
                    this->process_server_file_contents_request_pdu(
                        total_length, flags, chunk, header);
            break;

            case RDPECLIP::CB_FILECONTENTS_RESPONSE: {
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "File Contents Response PDU");
                }

                this->process_message_filecontents_response(
                    total_length, flags, chunk, header,
                    this->server_data, this->client_data);
            }
            break;

            case RDPECLIP::CB_FORMAT_DATA_REQUEST:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Format Data Request PDU");
                }

                send_message_to_client =
                    this->process_server_format_data_request_pdu(
                        total_length, flags, chunk, header);
            break;

            case RDPECLIP::CB_FORMAT_DATA_RESPONSE:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Format Data Response PDU");
                }

                send_message_to_client =
                    this->process_server_format_data_response_pdu(
                        total_length, flags, chunk, header);

                if (send_message_to_client &&
                    (flags & CHANNELS::CHANNEL_FLAG_FIRST)) {
                    this->update_exchanged_data(total_length);
                }
            break;

            case RDPECLIP::CB_FORMAT_LIST:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Format List PDU");
                }

                send_message_to_client =
                    this->process_server_format_list_pdu(
                        total_length, flags, chunk, header);

                if (this->format_list_notifier) {
                    if (!this->format_list_notifier->on_server_format_list()) {
                        this->format_list_notifier = nullptr;
                    }
                }
            break;

            case RDPECLIP::CB_FORMAT_LIST_RESPONSE:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Format List Response PDU");
                }

                if (this->clipboard_initialize_notifier) {
                    if (!this->clipboard_initialize_notifier->on_clipboard_initialize()) {
                        this->clipboard_initialize_notifier = nullptr;
                    }
                }
                else if (this->format_list_response_notifier) {
                    if (!this->format_list_response_notifier->on_server_format_list_response()) {
                        this->format_list_response_notifier = nullptr;
                    }
                }
            break;

            case RDPECLIP::CB_LOCK_CLIPDATA: {
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Lock Clipboard Data PDU");
                }

                this->process_message_lock_clipdata(
                    chunk, header, this->client_data, "client");
            }
            break;

            case RDPECLIP::CB_MONITOR_READY:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Monitor Ready PDU");
                }

                send_message_to_client =
                    this->process_server_monitor_ready_pdu(
                        total_length, flags, chunk, header);
            break;

            case RDPECLIP::CB_UNLOCK_CLIPDATA: {
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Unlock Clipboard Data PDU");
                }

                this->process_message_unlock_clipdata(
                    chunk, header, this->client_data, "server");
            }
            break;

            default:
                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_server_message: "
                            "Delivering unprocessed messages %s(%u) to client.",
                        RDPECLIP::get_msgType_name(this->server_data.message_type),
                        static_cast<unsigned>(this->server_data.message_type));
                }
            break;
        }   // switch (this->server_data.message_type)

        if (send_message_to_client) {
            this->send_message_to_client(total_length, flags, chunk_data,
                chunk_data_length);
        }   // switch (this->server_data.message_type)
    }   // process_server_message

    void set_session_probe_launcher(SessionProbeLauncher* launcher) {
        this->clipboard_monitor_ready_notifier = launcher;
        this->clipboard_initialize_notifier    = launcher;
        this->format_list_notifier             = launcher;
        this->format_list_response_notifier    = launcher;
        this->format_data_request_notifier     = launcher;
    }

    void empty_client_clipboard() {
        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::empty_client_clipboard");
        }

        RDPECLIP::CliprdrHeader clipboard_header(RDPECLIP::CB_FORMAT_LIST,
            RDPECLIP::CB_RESPONSE__NONE_, 0);

        StaticOutStream<256> out_s;

        clipboard_header.emit(out_s);

        const size_t totalLength = out_s.get_offset();

        this->send_message_to_server(
            totalLength,
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST,
            out_s.get_data(),
            totalLength);
    }

private:
    void process_format_list_response_disabled(VirtualChannelDataSender* sender)
    {
        RDPECLIP::FormatListResponsePDU pdu;

        RDPECLIP::CliprdrHeader header(RDPECLIP::CB_FORMAT_LIST_RESPONSE, RDPECLIP::CB_RESPONSE_OK, pdu.size());

        StaticOutStream<256> out_stream;

        header.emit(out_stream);
        pdu.emit(out_stream);

        if (sender)
        {
            const uint32_t total_length      = out_stream.get_offset();
            const uint32_t flags             =
                CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST;
            const uint8_t* chunk_data        = out_stream.get_data();
            const uint32_t chunk_data_length = total_length;

            (*sender)(total_length, flags, chunk_data, chunk_data_length);
        }
    }

    void process_format_list_response(
        InStream& chunk, RDPECLIP::CliprdrHeader const& in_header,
        ClipboardData& clipboard_data, char const* log_type)
    {
        auto check_remaining = [&](unsigned const expected){
            if (!chunk.in_check_rem(expected)) {
                LOG(LOG_WARNING,
                    "ClipboardVirtualChannel::process_%s_format_list_pdu: "
                        "Truncated (SHORT) CLIPRDR_FORMAT_LIST, "
                        "need=%u remains=%zu",
                    log_type, expected, chunk.in_remain());
                return false;
            }
            return true;
        };


        auto extract_utf8_data = [&](
            auto size_of_utf8_string,
            const uint32_t formatId,
            const size_t format_name_length
        ){
            uint8_t utf8_string[size_of_utf8_string + 1];
            const size_t length_of_utf8_string = ::UTF16toUTF8(
                chunk.get_current(), format_name_length, utf8_string,
                size_of_utf8_string);
            //utf8_string[length_of_utf8_string] = '\0';

            if (bool(this->verbose & RDPVerbose::cliprdr)) {
                LOG(LOG_INFO,
                    "ClipboardVirtualChannel::process_%s_format_list_pdu: "
                        "formatId=%s(%u) wszFormatName=\"%s\"",
                    log_type, RDPECLIP::get_FormatId_name(formatId),
                    formatId, utf8_string);
            }

            this->format_name_inventory[formatId] = ::char_ptr_cast(utf8_string);

            if (((sizeof(FILE_LIST_FORMAT_NAME) - 1) == length_of_utf8_string) &&
                !memcmp(FILE_LIST_FORMAT_NAME, utf8_string, length_of_utf8_string)) {
                clipboard_data.file_list_format_id = formatId;
            }
        };


        this->format_name_inventory.clear();

        if (!this->client_data.use_long_format_names || !this->server_data.use_long_format_names) {
            if (bool(this->verbose & RDPVerbose::cliprdr)) {
                LOG(LOG_INFO,
                    "ClipboardVirtualChannel::process_%s_format_list_pdu: "
                        "Short Format Name%s variant of Format List PDU is used "
                        "for exchanging updated format names.",
                    log_type,
                    ((in_header.msgFlags() & RDPECLIP::CB_ASCII_NAMES) ? " (ASCII 8)" : ""));
            }

            uint32_t remaining_data_length = in_header.dataLen();
            while (remaining_data_length)
            {
                // formatId(4) + formatName(32)
                if (!check_remaining(36)) {
                    break;
                }

                const     uint32_t formatId           = chunk.in_uint32_le();
                constexpr size_t   format_name_length =
                    32      // formatName(32)
                    / 2     // size_of(Unicode characters)(2)
                ;

                constexpr size_t size_of_utf8_string =
                    format_name_length *
                    maximum_length_of_utf8_character_in_bytes;

                extract_utf8_data(
                    std::integral_constant<std::size_t, size_of_utf8_string>{},
                    formatId, format_name_length);

                remaining_data_length -=
                    4       // formatId(4)
                    + 32    // formatName(32)
                ;

                chunk.in_skip_bytes(
                    32      // formatName(32)
                );
            }
        }
        else {
            if (bool(this->verbose & RDPVerbose::cliprdr)) {
                LOG(LOG_INFO,
                    "ClipboardVirtualChannel::process_%s_format_list_pdu: "
                        "Long Format Name variant of Format List PDU is used "
                        "for exchanging updated format names.", log_type);
            }

            uint32_t remaining_data_length = in_header.dataLen();
            while (remaining_data_length)
            {
                // formatId(4) + min_len(formatName)(2)
                if (!check_remaining(36)) {
                    break;
                }

                const size_t max_length_of_format_name = 256;

                const uint32_t formatId                    =
                    chunk.in_uint32_le();
                const size_t   format_name_length          =
                    ::UTF16StrLen(chunk.get_current()) + 1;
                const size_t   adjusted_format_name_length =
                    std::min(format_name_length - 1,
                             max_length_of_format_name);

                constexpr size_t size_of_utf8_string =
                    max_length_of_format_name *
                    maximum_length_of_utf8_character_in_bytes;

                extract_utf8_data(
                    std::integral_constant<std::size_t, size_of_utf8_string>{},
                    formatId, adjusted_format_name_length);

                remaining_data_length -=
                    4                        /* formatId(4) */
                    + format_name_length * 2 /* wszFormatName(variable) */
                ;

                chunk.in_skip_bytes(format_name_length * 2);
            }
        }
    }

    void process_message_filecontents_response(
        uint32_t total_length, uint32_t flags,
        InStream& chunk, RDPECLIP::CliprdrHeader const& header,
        ClipboardData& data_from, ClipboardData& data_to)
    {
        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
            this->update_exchanged_data(total_length);

            data_from.data_len = header.dataLen();

            if (data_from.data_len >= 4) {
                data_from.stream_id = chunk.in_uint32_le();
            }
        }

        auto file_contents_request_it
          = data_to.file_contents_request_info_inventory.find(data_from.stream_id);

        if (data_to.file_contents_request_info_inventory.end() != file_contents_request_it)
        {
            file_contents_request_info& file_contents_request = file_contents_request_it->second;

            file_info_inventory_type& file_info_inventory
                = data_from.file_stream_data_inventory[file_contents_request.clipDataId];

            file_info_type& file_info = file_info_inventory[file_contents_request.lindex];

            uint64_t const file_contents_request_position_current = file_contents_request.position + file_contents_request.offset;

            if (chunk.in_remain()) {
                if (file_info.sequential_access_offset == file_contents_request_position_current) {
                    uint32_t const length_ = std::min({
                            static_cast<uint32_t>(chunk.in_remain()),
                            static_cast<uint32_t>(file_info.size - file_info.sequential_access_offset),
                            file_contents_request.cbRequested - file_contents_request.offset
                        });

                    file_info.sha256.update({ chunk.get_current(), length_ });

                    file_contents_request.offset       += length_;
                    file_info.sequential_access_offset += length_;

                    if (file_info.sequential_access_offset == file_info.size) {
                        const bool from_remote_session = (&data_from == &this->server_data);
                        this->log_file_info(file_info, from_remote_session);
                    }
                }
            }
        }
    }

    void process_message_lock_clipdata(
        InStream& chunk, RDPECLIP::CliprdrHeader const& header,
        ClipboardData& clipboard_data, char const* log_type)
    {
        if (auto r = this->extract_clip_data_id(chunk, header, log_type)) {
            clipboard_data.clip_data_id                             = r.clipDataId;
            clipboard_data.file_stream_data_inventory[r.clipDataId] = file_info_inventory_type();
        }
    }

    void process_message_unlock_clipdata(
        InStream& chunk, RDPECLIP::CliprdrHeader const& header,
        ClipboardData& clipboard_data, char const* log_type)
    {
        if (auto r = this->extract_clip_data_id(chunk, header, log_type)) {
            clipboard_data.file_stream_data_inventory.erase(r.clipDataId);
        }
    }

    struct CheckClipboardHeaderResult
    {
        bool is_ok;
        uint32_t clipDataId;

        explicit operator bool () const noexcept
        {
            return is_ok;
        }
    };

    CheckClipboardHeaderResult extract_clip_data_id(
        InStream& chunk, RDPECLIP::CliprdrHeader const& header,
        char const* log_type)
    {
        const unsigned int expected = 4; // clipDataId(4)

        if (header.dataLen() >= expected) {
            if (!chunk.in_check_rem(expected)) {
                LOG(LOG_ERR,
                    "ClipboardVirtualChannel::process_%s_message: "
                        "Truncated CLIPRDR_LOCK_CLIPDATA, "
                        "need=%u remains=%zu",
                    log_type, expected, chunk.in_remain());
                throw Error(ERR_RDP_DATA_TRUNCATED);
            }

            uint32_t const clipDataId = chunk.in_uint32_le();

            if (bool(this->verbose & RDPVerbose::cliprdr)) {
                LOG(LOG_INFO,
                    "ClipboardVirtualChannel::process_%s_message: "
                        "clipDataId=%u", log_type, clipDataId);
            }

            return {true, clipDataId};
        }

        return {false, 0};
    }

    void process_file_contents_request(
        VirtualChannelDataSender* sender,
        uint32_t dwFlags, uint32_t streamId, char const* log_type)
    {
        if (!sender) {
            return ;
        }

        StaticOutStream<256> out_stream;

        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::process_%s_file_contents_request: "
                    "Requesting the contents of server file is denied.", log_type);
        }

        switch (dwFlags) {
            case RDPECLIP::FILECONTENTS_RANGE:
                {
                    RDPECLIP::FileContentsResponseRange pdu(streamId);
                    RDPECLIP::CliprdrHeader header( RDPECLIP::CB_FILECONTENTS_RESPONSE,
                                                    RDPECLIP::CB_RESPONSE_FAIL,
                                                    pdu.size());
                    header.emit(out_stream);
                    pdu.emit(out_stream);
                }
                break;

            case RDPECLIP::FILECONTENTS_SIZE:
                {
                    RDPECLIP::FileContentsResponseSize pdu(streamId, 0);
                    RDPECLIP::CliprdrHeader header( RDPECLIP::CB_FILECONTENTS_RESPONSE,
                                                    RDPECLIP::CB_RESPONSE_FAIL,
                                                    pdu.size());
                    header.emit(out_stream);
                    pdu.emit(out_stream);
                }
                break;
        }

        const uint32_t total_length      = out_stream.get_offset();
        const uint32_t flags             =
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST;
        const uint8_t* chunk_data        = out_stream.get_data();
        const uint32_t chunk_data_length = total_length;

        (*sender)(total_length, flags, chunk_data, chunk_data_length);
    }

    static void set_file_contens_request_info(
        ClipboardData& clipboard_data,
        RDPECLIP::FileContentsRequestPDU const& file_contents_request_pdu)
    {
        if (RDPECLIP::FILECONTENTS_RANGE == file_contents_request_pdu.dwFlags()
         && file_contents_request_pdu.has_optional_clipDataId()
        ) {
            clipboard_data.file_contents_request_info_inventory
                [file_contents_request_pdu.streamId()] = {
                    file_contents_request_pdu.lindex(),
                    file_contents_request_pdu.position(),
                    file_contents_request_pdu.cbRequested(),
                    file_contents_request_pdu.clipDataId(),
                    0 // offset
                };
        }
    }

    void process_format_data_request_send(
        VirtualChannelDataSender* sender, char const* log_type)
    {
        if (!sender) {
            return ;
        }

        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            LOG(LOG_INFO,
                "ClipboardVirtualChannel::process_%s_format_data_request_pdu: "
                    "Client to server Clipboard operation is not allowed.", log_type);
        }

        StaticOutStream<256> out_stream;
        RDPECLIP::FormatDataResponsePDU pdu;
        RDPECLIP::CliprdrHeader header(RDPECLIP::CB_FORMAT_DATA_RESPONSE, RDPECLIP::CB_RESPONSE_FAIL, 0);
        header.emit(out_stream);
        pdu.emit(out_stream, nullptr, 0);

        const uint32_t total_length      = out_stream.get_offset();
        const uint32_t flags             =
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST;
        const uint8_t* chunk_data        = out_stream.get_data();
        const uint32_t chunk_data_length = total_length;

        (*sender)(total_length, flags, chunk_data, chunk_data_length);

    }

    void proccess_format_data_response_pdu(
        uint32_t flags, InStream chunk, const RDPECLIP::CliprdrHeader & in_header,
        ClipboardData& clipboard_data)
    {
        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
            if (in_header.msgFlags() & RDPECLIP::CB_RESPONSE_OK) {
                this->process_format_data_response_first(chunk, in_header);
            }
        }

        if (this->client_data.file_list_format_id
         && this->requestedFormatId == this->client_data.file_list_format_id
        ) {
            if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
                if (!(in_header.msgFlags() & RDPECLIP::CB_RESPONSE_FAIL) && (in_header.dataLen() >= 4 /* cItems(4) */)) {
                    const uint32_t cItems = chunk.in_uint32_le();

                    if (!this->param_dont_log_data_into_syslog) {
                        LOG(LOG_INFO,
                            "Sending %sFileGroupDescriptorW(%u) clipboard data to server. "
                                "cItems=%u",
                            ((flags & CHANNELS::CHANNEL_FLAG_LAST) ?
                                "" : "(chunked) "),
                            this->client_data.file_list_format_id, cItems);
                    }
                }
            }
            else if (this->file_descriptor_stream.get_offset()) {
                this->update_file_contents_request_inventory(
                    this->file_descriptor_from_stream(chunk, this->file_descriptor_stream),
                    clipboard_data);

                this->file_descriptor_stream.rewind();
            }

            while (chunk.in_remain() >= RDPECLIP::FileDescriptor::size()) {
                RDPECLIP::FileDescriptor fd;
                fd.receive(chunk);

                if (bool(this->verbose & RDPVerbose::cliprdr)) {
                    fd.log(LOG_INFO);
                }

                this->update_file_contents_request_inventory(std::move(fd), clipboard_data);
            }

            if (chunk.in_remain()) {
                this->file_descriptor_stream.rewind();

                this->file_descriptor_stream.out_copy_bytes(
                    chunk.get_current(), chunk.in_remain());

                chunk.in_skip_bytes(chunk.in_remain());
            }

            if (flags & CHANNELS::CHANNEL_FLAG_LAST) {
                this->requestedFormatId = 0;
            }
        }
    }

    RDPECLIP::FileDescriptor file_descriptor_from_stream(
        InStream& chunk, OutStream& file_descriptor_stream)
    {
        const uint32_t complementary_data_length =
            RDPECLIP::FileDescriptor::size() -
                file_descriptor_stream.get_offset();

        assert(chunk.in_remain() >= complementary_data_length);

        file_descriptor_stream.out_copy_bytes(chunk.get_current(),
            complementary_data_length);

        chunk.in_skip_bytes(complementary_data_length);

        RDPECLIP::FileDescriptor fd;

        InStream in_stream(
            file_descriptor_stream.get_data(),
            file_descriptor_stream.get_offset()
        );
        fd.receive(in_stream);
        if (bool(this->verbose & RDPVerbose::cliprdr)) {
            fd.log(LOG_INFO);
        }

        return fd;
    }

    std::string format_data_response_receive(InStream& chunk, bool returns_string)
    {
        switch (this->requestedFormatId) {
            /*
            case RDPECLIP::CF_TEXT:
            {
                const size_t length_of_data_to_dump = std::min(
                    chunk.in_remain(), max_length_of_data_to_dump);
                const std::string data_to_dump(
                    ::char_ptr_cast(chunk.get_current()),
                    length_of_data_to_dump);
                LOG(LOG_INFO, "%s", data_to_dump);
            }
            break;
            */
            case RDPECLIP::CF_UNICODETEXT: {
                assert(!(chunk.in_remain() & 1));

                constexpr size_t const max_length_of_data_to_dump = 256;

                const size_t length_of_data_to_dump = std::min(
                    chunk.in_remain(), max_length_of_data_to_dump * 2);

                constexpr size_t size_of_utf8_string =
                    max_length_of_data_to_dump *
                        maximum_length_of_utf8_character_in_bytes;

                uint8_t utf8_string[size_of_utf8_string + 1] {};
                const size_t length_of_utf8_string = ::UTF16toUTF8(
                    chunk.get_current(), length_of_data_to_dump / 2,
                    utf8_string, size_of_utf8_string);
                return returns_string
                    ? std::string(
                        char_ptr_cast(utf8_string),
                        ((length_of_utf8_string && !utf8_string[length_of_utf8_string]) ?
                        length_of_utf8_string - 1 :
                        length_of_utf8_string))
                    : std::string();
            }

            case RDPECLIP::CF_LOCALE: {
                const uint32_t locale_identifier = chunk.in_uint32_le();
                return returns_string ? std::to_string(locale_identifier) : std::string();
            }
        }

        return std::string{};
    }

    void process_format_data_response_first(
        InStream& chunk, const RDPECLIP::CliprdrHeader& in_header)
    {
        std::string const& format_name_ = this->format_name_inventory[this->requestedFormatId];

        bool const log_current_activity = (
            (!this->param_log_only_relevant_clipboard_activities) ||
            (0 != strcasecmp("Preferred DropEffect", format_name_.c_str()) &&
                0 != strcasecmp("FileGroupDescriptorW", format_name_.c_str()))
        );

        bool const has_log = (log_current_activity || !this->param_dont_log_data_into_wrm);

        std::string const data_to_dump = this->format_data_response_receive(chunk, has_log);

        if (!has_log) {
            return ;
        }

        auto format_name = format_name_.empty()
          ? std::string(RDPECLIP::get_FormatId_name(this->requestedFormatId))
          : std::string(format_name_);
        format_name += "(";
        format_name += std::to_string(this->requestedFormatId);
        format_name += ")";

        char const* type = (data_to_dump.empty() ?
            "CB_COPYING_PASTING_DATA_TO_REMOTE_SESSION" :
            "CB_COPYING_PASTING_DATA_TO_REMOTE_SESSION_EX");

        auto const size_str = std::to_string(in_header.dataLen());


        if (log_current_activity) {
            std::string info;
            ::key_qvalue_pairs(
                    info,
                    {
                        { "type", type },
                        { "format", format_name },
                        { "size", size_str }
                    }
                );
            if (!data_to_dump.empty()) {
                ::key_qvalue_pairs(
                        info,
                        {
                            { "partial_data", data_to_dump }
                        }
                    );
            }

            ArcsightLogInfo arc_info;
            arc_info.name = type;
            arc_info.ApplicationProtocol = "rdp";
            arc_info.message = info;
            arc_info.direction_flag = ArcsightLogInfo::SERVER_DST;
            this->report_message.log6(info, arc_info, tvtime());

            if (!this->param_dont_log_data_into_syslog) {
                LOG(LOG_INFO, "%s", info);
            }
        }

        if (!this->param_dont_log_data_into_wrm) {
            std::string message(type);
            message += "=";
            message += format_name;
            message += "\x01";
            message += size_str;
            if (!data_to_dump.empty()) {
                message += "\x01";
                message += data_to_dump;
            }

            this->front.session_update(message);
        }
    }

    void update_file_contents_request_inventory(
        RDPECLIP::FileDescriptor&& fd, ClipboardData& clipboard_data)
    {
        file_info_inventory_type& file_info_inventory =
            clipboard_data.file_stream_data_inventory[clipboard_data.clip_data_id];

        file_info_inventory.push_back({
            std::move(fd.file_name), fd.file_size(), 0, SslSha256()
        });
    }
};  // class ClipboardVirtualChannel
