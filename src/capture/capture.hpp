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
   Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/

#ifndef _REDEMPTION_CAPTURE_CAPTURE_HPP_
#define _REDEMPTION_CAPTURE_CAPTURE_HPP_

#include "core/wait_obj.hpp"

#include "gdi/graphic_api.hpp"
#include "gdi/capture_api.hpp"
#include "gdi/kbd_input_api.hpp"
#include "gdi/input_pointer_api.hpp"
#include "gdi/capture_probe_api.hpp"

#include "utils/pattutils.hpp"

#include "capture/utils/graphic_capture_impl.hpp"
#include "capture/utils/wrm_capture_impl.hpp"
#include "capture/utils/kbd_capture_impl.hpp"
#include "capture/utils/image_capture_impl.hpp"
#include "capture/utils/capture_apis_impl.hpp"

class Capture final
: public gdi::GraphicBase<Capture>
, public gdi::CaptureApi
, public gdi::KbdInputApi
, public gdi::MouseInputApi
, public gdi::CaptureProbeApi
, public gdi::ExternalCaptureApi
, public gdi::UpdateConfigCaptureApi
{
    using Graphic = GraphicCaptureImpl;

    using Static = ImageCaptureImpl;

    using Native = WrmCaptureImpl;

    using Kbd = KbdCaptureImpl;

public:
    const bool capture_wrm;
    const bool capture_png;
// for extension
// end extension

private:
// for extension
// end extension

    std::unique_ptr<Graphic> gd;
    std::unique_ptr<Native> pnc;
    std::unique_ptr<Static> psc;
    std::unique_ptr<Kbd> pkc;

    CaptureApisImpl::Capture capture_api;
    CaptureApisImpl::KbdInput kbd_input_api;
    CaptureApisImpl::MouseInput mouse_input_api;
    CaptureApisImpl::CaptureProbe capture_probe_api;
    CaptureApisImpl::ExternalCapture external_capture_api;
    CaptureApisImpl::UpdateConfigCapture update_config_capture_api;
    Graphic::GraphicApi * graphic_api = nullptr;

    ApisRegister get_apis_register() {
        return {
            this->graphic_api ? &this->graphic_api->gds : nullptr,
            this->graphic_api ? &this->graphic_api->snapshoters : nullptr,
            this->capture_api.caps,
            this->kbd_input_api.kbds,
            this->mouse_input_api.mouses,
            this->capture_probe_api.probes,
            this->external_capture_api.objs,
            this->update_config_capture_api.objs,
        };
    };

public:
    Capture(
        const timeval & now,
        uint16_t width, uint16_t height, int order_bpp, int capture_bpp,
        bool enable_rt, bool no_timestamp, auth_api * authentifier,
        const Inifile & ini, Random & rnd, CryptoContext & cctx,
        bool full_video)
    : capture_wrm(bool(ini.get<cfg::video::capture_flags>() & configs::CaptureFlags::wrm))
    , capture_png(ini.get<cfg::video::png_limit>() > 0)
    , capture_api(now, width / 2, height / 2)
    {
        TODO("Remove that after change of capture interface")
        (void)full_video;

        bool const enable_kbd
          = !bool(ini.get<cfg::video::disable_keyboard_log>() & configs::KeyboardLogFlags::syslog)
          || (ini.get<cfg::session_log::enable_session_log>() &&
              (ini.get<cfg::session_log::keyboard_input_masking_level>()
               != ::configs::KeyboardInputMaskingLevel::fully_masked)
             )
          || ::contains_kbd_pattern(ini.get<cfg::context::pattern_kill>().c_str())
          || ::contains_kbd_pattern(ini.get<cfg::context::pattern_notify>().c_str())
        ;

        if (ini.get<cfg::debug::capture>()) {
            LOG(LOG_INFO, "Enable capture:  wrm=%d  png=%d  kbd=%d",
                this->capture_wrm ? 1 : 0,
                this->capture_png ? 1 : 0,
                enable_kbd ? 1 : 0
            );
        }

        const int groupid = ini.get<cfg::video::capture_groupid>(); // www-data
        const bool capture_drawable = this->capture_wrm || this->capture_png;
        const char * record_tmp_path = ini.get<cfg::video::record_tmp_path>();
        const char * record_path = ini.get<cfg::video::record_path>();
        const char * hash_path = ini.get<cfg::video::hash_path>();

        char path[1024];
        char basename[1024];
        char extension[128];
        strcpy(path, WRM_PATH "/");     // default value, actual one should come from movie_path
        strcpy(basename, "redemption"); // default value actual one should come from movie_path
        strcpy(extension, "");          // extension is currently ignored

        if (!canonical_path(
            ini.get<cfg::globals::movie_path>().c_str()
          , path, sizeof(path)
          , basename, sizeof(basename)
          , extension, sizeof(extension))
        ) {
            LOG(LOG_ERR, "Buffer Overflowed: Path too long");
            throw Error(ERR_RECORDER_FAILED_TO_FOUND_PATH);
        }

        if (capture_drawable) {
            this->gd.reset(new Graphic(width, height, order_bpp, this->capture_api.mouse_trace()));
            this->graphic_api = &this->gd->get_graphic_api();
            this->capture_api.set_drawable(&this->gd->impl());

            if (this->capture_png) {
                if (recursive_create_directory(record_tmp_path, S_IRWXU | S_IRWXG, groupid) != 0) {
                    LOG(LOG_ERR, "Failed to create directory: \"%s\"", record_tmp_path);
                }

                this->psc.reset(new Static(
                    now, enable_rt, authentifier, this->gd->impl(),
                    record_tmp_path, basename, groupid, ini
                ));
            }

            if (this->capture_wrm) {
                if (recursive_create_directory(record_path, S_IRWXU | S_IRGRP | S_IXGRP, groupid) != 0) {
                    LOG(LOG_ERR, "Failed to create directory: \"%s\"", record_path);
                }

                if (recursive_create_directory(hash_path, S_IRWXU | S_IRGRP | S_IXGRP, groupid) != 0) {
                    LOG(LOG_ERR, "Failed to create directory: \"%s\"", hash_path);
                }

                this->pnc.reset(new Native(
                    now, capture_bpp, ini.get<cfg::globals::trace_type>(),
                    cctx, record_path, hash_path, basename,
                    groupid, authentifier, this->gd->rdp_drawable(), ini
                ));
            }
        }

        if (enable_kbd) {
            this->pkc.reset(new Kbd(now, authentifier, ini));
        }

        ApisRegister apis_register = this->get_apis_register();

        if (this->gd ) { this->gd->attach_apis (apis_register, ini); }
        if (this->pnc) { this->pnc->attach_apis(apis_register, ini); }
        if (this->psc) { this->psc->attach_apis(apis_register, ini); }
        if (this->pkc) { this->pkc->attach_apis(apis_register, ini); }

        if (this->gd ) { this->gd->start(); }
    }

    ~Capture() override {
        this->pkc.reset();

        this->psc.reset();

        if (this->pnc) {
            timeval now = tvtime();
            this->pnc->send_timestamp_chunk(now, false);
            this->pnc.reset();
        }

        this->gd.reset();
    }

    wait_obj & get_capture_event() {
        return this->capture_api.get_capture_event();
    }

    void update_config(const Inifile & ini) override {
        this->update_config_capture_api.update_config(ini);
    }

    bool kbd_input(const timeval & now, uint32_t uchar) override {
        return this->kbd_input_api.kbd_input(now, uchar);
    }

    void update_pointer_position(uint16_t x, uint16_t y) override {
        this->mouse_input_api.update_pointer_position(x, y);
    }

    void enable_kbd_input_mask(bool enable) override {
        this->kbd_input_api.enable_kbd_input_mask(enable);
    }

private:
    std::chrono::microseconds do_snapshot(
        const timeval & now,
        int x, int y,
        bool ignore_frame_in_timeval
    ) override {
        return this->capture_api.snapshot(now, x, y, ignore_frame_in_timeval);
    }

    void do_pause_capture(timeval const & now) override {
        this->capture_api.pause_capture(now);
    }

    void do_resume_capture(timeval const & now) override {
        this->capture_api.resume_capture(now);
    }

    friend gdi::GraphicCoreAccess;

    template<class... Ts>
    void draw_impl(Ts const & ... args) {
        if (this->graphic_api) {
            this->graphic_api->draw(args...);
        }
    }

public:
    void set_pointer(const Pointer & cursor) override {
        if (this->graphic_api) {
            this->graphic_api->set_pointer(cursor);
        }
    }

    void set_palette(const BGRPalette & palette) override {
        if (this->graphic_api) {
            this->graphic_api->set_palette(palette);
        }
    }

    void sync() override {
        if (this->graphic_api) {
            this->graphic_api->sync();
        }
    }

    void set_row(size_t rownum, const uint8_t * data) override {
        if (this->gd){
            this->gd->rdp_drawable().set_row(rownum, data);
        }
    }

    gdi::GraphicApi * get_graphic_api() const {
        return this->graphic_api;
    }

    void add_graphic(gdi::GraphicApi & gd) {
        if (this->graphic_api) {
            this->get_apis_register().graphic_list->push_back(gd);
            // TODO
            this->gd->start();
        }
    }

    void set_order_bpp(uint8_t order_bpp) {
        if (this->graphic_api) {
            this->gd->update_order_bpp(order_bpp);
        }
    }

    void set_pointer_display() /*override*/ {
        if (this->gd) {
            this->gd->rdp_drawable().show_mouse_cursor(false);
        }
    }

    // toggles externally genareted breakpoint.
    void external_breakpoint() override {
        this->external_capture_api.external_breakpoint();
    }

    void external_time(const timeval & now) override {
        this->external_capture_api.external_time(now);
    }

    void session_update(const timeval & now, array_view_const_char message) override {
        this->capture_probe_api.session_update(now, message);
    }

    void possible_active_window_change() override {
        this->capture_probe_api.possible_active_window_change();
    }

    void zoom(unsigned percent) {
        assert(this->psc);
        this->psc->zoom(percent);
    }
};

#endif
