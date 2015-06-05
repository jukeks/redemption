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

#ifndef _REDEMPTION_MOD_RDP_RDP_ASYNCHRONOUS_TASK_HPP_
#define _REDEMPTION_MOD_RDP_RDP_ASYNCHRONOUS_TASK_HPP_

#include "asynchronous_task_manager.hpp"
#include "RDP/channels/rdpdr_file_system_drive_manager.hpp"
#include "transport.hpp"
#include "wait_obj.hpp"

class RdpdrDriveReadTask : public AsynchronousTask {
    std::unique_ptr<Transport> transport;

    const int file_descriptor;

    ToServerSender & to_server_sender;

    const uint32_t DeviceId;
    const uint32_t CompletionId;

    const uint32_t total_number_of_bytes_to_read;
          uint32_t remaining_number_of_bytes_to_read = 0;

    uint32_t length = 0;

    const uint32_t verbose;

public:
    RdpdrDriveReadTask(std::unique_ptr<Transport> transport,
                       int file_descriptor,
                       ToServerSender & to_server_sender,
                       uint32_t DeviceId,
                       uint32_t CompletionId,
                       uint32_t number_of_bytes_to_read,
                       uint32_t verbose = 0)
    : AsynchronousTask()
    , transport(std::move(transport))
    , file_descriptor(file_descriptor)
    , to_server_sender(to_server_sender)
    , DeviceId(DeviceId)
    , CompletionId(CompletionId)
    , total_number_of_bytes_to_read(number_of_bytes_to_read)
    , remaining_number_of_bytes_to_read(number_of_bytes_to_read)
    , verbose(verbose) {}

    virtual void configure_wait_object(wait_obj & wait_object) const override {
        REDASSERT(!wait_object.waked_up_by_time);

        wait_object.object_and_time = true;

        wait_object.set(1000000);
    }

    virtual int get_file_descriptor() const override { return this->file_descriptor; }

    virtual bool run(const wait_obj & wait_object) override {
        if (wait_object.waked_up_by_time) {
            LOG(LOG_WARNING, "RdpdrDriveReadTask::run: File (%d) is not ready!",
                this->file_descriptor);
            return true;
        }

        BStream out_stream(CHANNELS::CHANNEL_CHUNK_LENGTH);

        uint32_t out_flags = 0;

        if (this->remaining_number_of_bytes_to_read == this->total_number_of_bytes_to_read) {
            const rdpdr::SharedHeader sh_s(rdpdr::Component::RDPDR_CTYP_CORE,
                                           rdpdr::PacketId::PAKID_CORE_DEVICE_IOCOMPLETION);
            sh_s.emit(out_stream);

            const rdpdr::DeviceIOResponse device_io_response(
                    DeviceId,
                    CompletionId,
                    0x00000000 /* STATUS_SUCCESS */
                );
            if (this->verbose) {
                LOG(LOG_INFO, "RdpdrDriveReadTask::run");
                device_io_response.log(LOG_INFO);
            }
            device_io_response.emit(out_stream);

            out_stream.out_uint32_le(this->total_number_of_bytes_to_read);  // length(4)

            if (this->verbose) {
                LOG(LOG_INFO, "RdpdrDriveReadTask::run: Length=%u",
                    remaining_number_of_bytes_to_read);
            }

            out_stream.mark_end();

            out_flags |= CHANNELS::CHANNEL_FLAG_FIRST;

            REDASSERT(!this->length);

            this->length = out_stream.size() + total_number_of_bytes_to_read;
        }

        const uint8_t * const saved_out_stream_p = out_stream.p;

        transport->recv(&out_stream.p,
            std::min<uint32_t>(out_stream.tailroom(), this->remaining_number_of_bytes_to_read));
        out_stream.mark_end();

        const uint32_t number_of_bytes_read = out_stream.p - saved_out_stream_p;

        if (this->verbose) {
            LOG(LOG_INFO, "RdpdrDriveReadTask::run: NumberOfBytesRead=%u",
                number_of_bytes_read);
        }

        this->remaining_number_of_bytes_to_read -= number_of_bytes_read;
        if (!this->remaining_number_of_bytes_to_read) {
            out_flags |= CHANNELS::CHANNEL_FLAG_LAST;
        }

        REDASSERT(this->length);

        to_server_sender(this->length, out_flags, out_stream.get_data(), out_stream.size());

        return (this->remaining_number_of_bytes_to_read != 0);
    }
};

#endif  // #ifndef _REDEMPTION_MOD_RDP_RDP_ASYNCHRONOUS_TASK_HPP_
