/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010-2013
   Author(s): Christophe Grosjean, Javier Caverni, Meng Tan, Raphael Zhou
*/

#ifndef _REDEMPTION_MOD_MOD_API_HPP_
#define _REDEMPTION_MOD_MOD_API_HPP_

#include <ctime>

#include "core/callback.hpp"
#include "core/font.hpp"
#include "core/wait_obj.hpp"
#include "core/RDP/caches/glyphcache.hpp"
#include "core/RDP/orders/RDPOrdersCommon.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryGlyphIndex.hpp"
#include "gdi/graphic_api.hpp"

class Inifile;

enum {
    BUTTON_STATE_UP   = 0,
    BUTTON_STATE_DOWN = 1
};

class mod_api : public Callback, public gdi::GraphicApi {
protected:
    wait_obj           event;
    RDPPen             pen;
    gdi::GraphicApi * gd;

    uint16_t front_width;
    uint16_t front_height;

public:
    mod_api(const uint16_t front_width, const uint16_t front_height)
    : gd(this)
    , front_width(front_width)
    , front_height(front_height) {
        this->event.set(0);
    }

    ~mod_api() override {}

    virtual wait_obj& get_event() { return this->event; }
    virtual wait_obj * get_secondary_event() { return nullptr; }

    virtual wait_obj * get_asynchronous_task_event(int & out_fd) { out_fd = -1; return nullptr; }
    virtual void process_asynchronous_task() {}

    virtual wait_obj * get_session_probe_launcher_event() { return nullptr; }
    virtual void process_session_probe_launcher() {}

    uint16_t get_front_width() const { return this->front_width; }
    uint16_t get_front_height() const { return this->front_height; }

protected:

    static gdi::GraphicApi * get_gd(mod_api const & mod)
    {
        return mod.gd;
    }

    static void set_gd(mod_api & mod, gdi::GraphicApi * gd)
    {
        mod.gd = gd;
    }

public:
    void server_draw_text_poubelle(Font const & font, int16_t x, int16_t y, const char * text,
                uint32_t fgcolor, uint32_t bgcolor, const Rect & clip)
    {
        gdi::server_draw_text(*this->gd, font, x, y, text, fgcolor, bgcolor, clip);
    }


    virtual void send_to_front_channel(const char * const mod_channel_name,
        uint8_t const * data, size_t length, size_t chunk_size, int flags) = 0;

    // draw_event is run when mod socket received some data (drawing order)
    // or auto-generated by modules, say to comply to some refresh.
    // draw event decodes incoming traffic from backend and eventually calls front to draw things
    // may raise an exception (say if connection to server is closed), but returns nothings
    virtual void draw_event(time_t now, const GraphicApi & drawable) = 0;

    // used when context changed to avoid creating a new module
    // it usually perform some task identical to what constructor does
    // henceforth it should often be called by constructors
    virtual void refresh_context(Inifile & ini) {}

    virtual bool is_up_and_running() { return false; }

    virtual void disconnect() {}

    virtual void display_osd_message(std::string & message) {}
};

#endif
