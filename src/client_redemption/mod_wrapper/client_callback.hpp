/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010-2013
   Author(s): Clément Moroldo, David Fort
*/

#pragma once

#include "utils/log.hpp"

#ifndef Q_MOC_RUN

#include "core/callback.hpp"
#include "keyboard/keymap2.hpp"
#include "core/channel_names.hpp"
#include "core/channel_list.hpp"
#include "client_redemption/client_redemption_api.hpp"
#include "client_redemption/client_input_output_api/client_keymap_api.hpp"

#endif


class ClientCallback
{
private:
    Keymap2           keymap;
    StaticOutStream<256> decoded_data;    // currently not initialised
    int                  _timer;

    mod_api            * mod = nullptr;
    ClientRedemptionAPI * client;

    ClientKeyLayoutAPI * keyLayout;

public:
    struct MouseData {
        uint16_t x = 0;
        uint16_t y = 0;
    } mouse_data;

    ClientCallback(ClientRedemptionAPI * client, ClientKeyLayoutAPI * keyLayout)
    :  _timer(0)
    , client(client)
    , keyLayout(keyLayout)
    {}

    // CHANNELS
    void send_to_mod_channel(CHANNELS::ChannelNameId channel_name, InStream & stream, size_t size, uint32_t flag) {
        this->mod->send_to_mod_channel(channel_name, stream, size, flag);
    }

    void process_client_channel_out_data(const CHANNELS::ChannelNameId & front_channel_name, const uint64_t total_length, OutStream & out_stream_first_part, const size_t first_part_data_size,  const_bytes_view data, uint32_t flags){

        // 3.1.5.2.2.1 Reassembly of Chunked Virtual Channel Dat

        // Virtual channel data can span multiple Virtual Channel PDUs (section 3.1.5.2.1).
        // If this is the case, the embedded length field of the channelPduHeader field
        // (the Channel PDU Header structure is specified in section 2.2.6.1.1) specifies
        // the total length of the uncompressed virtual channel data spanned across all of
        // the associated Virtual Channel PDUs. This length is referred to as totalLength.
        // For example, assume that the virtual channel chunking size specified in the Virtual
        // Channel Capability Set (section 2.2.7.1.10) is 1,000 bytes and that 2,062 bytes need
        // to be transmitted on a given virtual channel. In this example,
        // the following sequence of Virtual Channel PDUs will be sent (only relevant fields are listed):

        //    Virtual Channel PDU 1:
        //    CHANNEL_PDU_HEADER::length = 2062 bytes
        //    CHANNEL_PDU_HEADER::flags = CHANNEL_FLAG_FIRST
        //    Actual virtual channel data is 1000 bytes (the chunking size).

        //    Virtual Channel PDU 2:
        //    CHANNEL_PDU_HEADER::length = 2062 bytes
        //    CHANNEL_PDU_HEADER::flags = 0
        //    Actual virtual channel data is 1000 bytes (the chunking size).

        //    Virtual Channel PDU 3:
        //    CHANNEL_PDU_HEADER::length = 2062 bytes
        //    CHANNEL_PDU_HEADER::flags = CHANNEL_FLAG_LAST
        //    Actual virtual channel data is 62 bytes.

    //     // The size of the virtual channel data in the last PDU (the data in the virtualChannelData field)
        // is determined by subtracting the offset of the virtualChannelData field in the encapsulating
        // Virtual Channel PDU from the total size specified in the tpktHeader field. This length is
        // referred to as chunkLength.

        // Upon receiving each Virtual Channel PDU, the server MUST dispatch the virtual channel data to
        // the appropriate virtual channel endpoint. The sequencing of the chunk (whether it is first,
        // intermediate, or last), totalLength, chunkLength, and the virtualChannelData fields MUST
        // be dispatched to the virtual channel endpoint so that the data can be correctly reassembled.
        // If the CHANNEL_FLAG_SHOW_PROTOCOL (0x00000010) flag is specified in the Channel PDU Header,
        // then the channelPduHeader field MUST also be dispatched to the virtual channel endpoint.

        // A reassembly buffer MUST be created by the virtual channel endpoint using the size specified
        // by totalLength when the first chunk is received. After the reassembly buffer has been created
        // the first chunk MUST be copied into the front of the buffer. Subsequent chunks MUST then be
        // copied into the reassembly buffer in the order in which they are received. Upon receiving the
        // last chunk of virtual channel data, the reassembled data is processed by the virtual channel endpoint.


        if (data.size() > first_part_data_size ) {

            int real_total = data.size() - first_part_data_size;
            const int cmpt_PDU_part(real_total  / CHANNELS::CHANNEL_CHUNK_LENGTH);
            const int remains_PDU  (real_total  % CHANNELS::CHANNEL_CHUNK_LENGTH);
            int data_sent(0);

            // First Part
                out_stream_first_part.out_copy_bytes(data.data(), first_part_data_size);

                data_sent += first_part_data_size;
                InStream chunk_first(out_stream_first_part.get_data(), out_stream_first_part.get_offset());

                this->send_to_mod_channel( front_channel_name
                                         , chunk_first
                                         , total_length
                                         , CHANNELS::CHANNEL_FLAG_FIRST | flags
                                         );

//             ::hexdump(out_stream_first_part.get_data(), out_stream_first_part.get_offset());


            for (int i = 0; i < cmpt_PDU_part; i++) {

            // Next Part
                StaticOutStream<CHANNELS::CHANNEL_CHUNK_LENGTH> out_stream_next_part;
                out_stream_next_part.out_copy_bytes(data.data() + data_sent, CHANNELS::CHANNEL_CHUNK_LENGTH);

                data_sent += CHANNELS::CHANNEL_CHUNK_LENGTH;
                InStream chunk_next(out_stream_next_part.get_data(), out_stream_next_part.get_offset());

                this->send_to_mod_channel( front_channel_name
                                         , chunk_next
                                         , total_length
                                         , flags
                                         );

//             ::hexdump(out_stream_next_part.get_data(), out_stream_next_part.get_offset());
            }

            // Last part
                StaticOutStream<CHANNELS::CHANNEL_CHUNK_LENGTH> out_stream_last_part;
                out_stream_last_part.out_copy_bytes(data.data() + data_sent, remains_PDU);

                InStream chunk_last(out_stream_last_part.get_data(), out_stream_last_part.get_offset());

                this->send_to_mod_channel( front_channel_name
                                         , chunk_last
                                         , total_length
                                         , CHANNELS::CHANNEL_FLAG_LAST | flags
                                         );

//         ::hexdump(out_stream_last_part.get_data(), out_stream_last_part.get_offset());

        } else {

            out_stream_first_part.out_copy_bytes(data.data(), data.size());
            InStream chunk(out_stream_first_part.get_data(), out_stream_first_part.get_offset());

            this->send_to_mod_channel( front_channel_name
                                     , chunk
                                     , total_length
                                     , CHANNELS::CHANNEL_FLAG_LAST | CHANNELS::CHANNEL_FLAG_FIRST |
                                       flags
                                     );
        }
    }


    // REPLAY
    void replay(const std::string & movie_name, const std::string & movie_dir) {
        this->client->replay(movie_name, movie_dir);
    }

    timeval reload_replay_mod(int begin, timeval now_stop) {
        return this->client->reload_replay_mod(begin, now_stop);
    }

    void replay_set_sync() {
        this->client->replay_set_sync();
    }

    bool is_replay_on() {
        return this->client->is_replay_on();
    }

    bool load_replay_mod(std::string const & movie_dir, std::string const & movie_name, timeval time_1, timeval time_2) {
        return this->client->load_replay_mod(movie_dir, movie_name, time_1, time_2);
    }

    void delete_replay_mod() {
        this->client->delete_replay_mod();
    }

    void closeFromScreen() {
        this->client->closeFromScreen();
    }

    time_t get_real_time_movie_begin() {
        return this->client->get_real_time_movie_begin();
    }

    void replay_set_pause(timeval time) {
        this->client->replay_set_pause(time);
    }

    time_t get_movie_time_length(char const * movie_path) {
        return this->client->get_movie_time_length(movie_path);
    }

    char const * get_mwrm_filename() {
        return this->client->get_mwrm_filename().c_str();
    }

    void instant_play_client(std::chrono::microseconds time) {
        this->client->instant_play_client(time);
    }



    void set_mod(mod_api * mod) {
        this->mod = mod;
    }

    void init_layout(int lcid) {
        this->keymap.init_layout(lcid);
    }
    // TODO string_view
    void keyPressEvent(const int key, std::string const& text) {
        this->keyLayout->init(0, key, text);
        int keyCode = this->keyLayout->get_scancode();
        if (keyCode != 0) {
            this->send_rdp_scanCode(keyCode, this->keyLayout->get_flag());
        }
    }

    // TODO string_view
    void keyReleaseEvent(const int key, std::string const& text) {
        this->keyLayout->init(KBD_FLAG_UP, key, text);
        int keyCode = this->keyLayout->get_scancode();
        if (keyCode != 0) {
            this->send_rdp_scanCode(keyCode, this->keyLayout->get_flag());
        }
    }

    bool connect() {
        return this->client->connect();
    }

    void disconnexionReleased() {
        this->client->disconnexionReleased();
    }

    void refreshPressed() {
//         if (this->mod != nullptr) {
//             Rect rect(0, 0, this->config.info.width, this->config.info.height);
//             this->mod->rdp_input_invalidate(rect);
//         }
    }

    void CtrlAltDelPressed() {
        int flag = Keymap2::KBDFLAGS_EXTENDED;

        this->send_rdp_scanCode(KBD_SCANCODE_ALTGR , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_CTRL  , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_DELETE, flag);
    }

    void CtrlAltDelReleased() {
        int flag = Keymap2::KBDFLAGS_EXTENDED | KBD_FLAG_UP;

        this->send_rdp_scanCode(KBD_SCANCODE_ALTGR , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_CTRL  , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_DELETE, flag);
    }

    void mouseButtonEvent(int x, int y, int flag) {
        if (this->mod != nullptr) {
            this->mod->rdp_input_mouse(flag, x, y, &(this->keymap));
        }
    }

    void wheelEvent(int  /*unused*/,  int  /*unused*/, int /*delta*/) {
        // int flag(MOUSE_FLAG_HWHEEL);
        // if (delta < 0) {
        //     flag = flag | MOUSE_FLAG_WHEEL_NEGATIVE;
        // }
        // if (this->mod != nullptr) {
        //     this->mod->rdp_input_mouse(flag, e->x(), e->y(), &(this->keymap));
        // }
    }

    bool mouseMouveEvent(int x, int y) {

        if (this->mod != nullptr /*&& y < this->config.info.height*/) {
            this->mouse_data.x = x;
            this->mouse_data.y = y;
            this->mod->rdp_input_mouse(MOUSE_FLAG_MOVE, this->mouse_data.x, this->mouse_data.y, &(this->keymap));
        }

        return false;
    }

    void send_rdp_scanCode(int keyCode, int flag) {
        bool tsk_switch_shortcuts = false;
        Keymap2::DecodedKeys decoded_keys = this->keymap.event(flag, keyCode, tsk_switch_shortcuts);
        switch (decoded_keys.count)
        {
        case 2:
            if (this->decoded_data.has_room(sizeof(uint32_t))) {
                this->decoded_data.out_uint32_le(decoded_keys.uchars[0]);
            }
            if (this->decoded_data.has_room(sizeof(uint32_t))) {
                this->decoded_data.out_uint32_le(decoded_keys.uchars[1]);
            }
            break;
        case 1:
            if (this->decoded_data.has_room(sizeof(uint32_t))) {
                this->decoded_data.out_uint32_le(decoded_keys.uchars[0]);
            }
            break;
        default:
        case 0:
            break;
        }
        if (this->mod != nullptr) {
            this->mod->rdp_input_scancode(keyCode, 0, flag, this->_timer, &(this->keymap));
        }
    }

    void send_rdp_unicode(uint16_t unicode, uint16_t flag) {
        this->mod->rdp_input_unicode(unicode, flag);
    }

    KeyCustomDefinition get_key_info(int keyCode, std::string const& text) {
        return this->keyLayout->get_key_info(keyCode, text);
    }

};
