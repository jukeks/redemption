/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2012
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFlatWabClose
#include <boost/test/auto_unit_test.hpp>

#undef FIXTURES_PATH
#define FIXTURES_PATH "./tests/fixtures"
#undef SHARE_PATH
#define SHARE_PATH "./tests/fixtures"

#define LOGNULL
#include "log.hpp"

#include "internal/widget2/flat_wab_close.hpp"
#include "internal/widget2/screen.hpp"
#include "png.hpp"
#include "ssl_calls.hpp"
#include "RDP/RDPDrawable.hpp"
#include "check_sig.hpp"


#undef OUTPUT_FILE_PATH
#define OUTPUT_FILE_PATH "/tmp/"

#include "fake_draw.hpp"

BOOST_AUTO_TEST_CASE(TraceFlatWabClose)
{
    BOOST_CHECK(1);

    TestDraw drawable(800, 600);

    BOOST_CHECK(1);

    // FlatWabClose is a flat_wab_close widget at position 0,0 in it's parent context
    WidgetScreen parent(drawable, 800, 600);
    NotifyApi * notifier = NULL;
    int id = 0;

    BOOST_CHECK(1);

    try {

        FlatWabClose flat_wab_close(drawable, 800, 600, parent, notifier,
                                    "abc<br>def", id, "rec", "rec");

    // ask to widget to redraw at it's current position
    flat_wab_close.rdp_input_invalidate(flat_wab_close.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close.png");

    } catch (Error & e) {
        LOG(LOG_INFO, "e=%u", e.id);
    };

    BOOST_CHECK(1);

    char message[1024];

    if (!check_sig(drawable.gd.drawable, message,
                   "\xeb\x51\xf6\xe4\x19\x6c\x20\x50\xae\x33"
                   "\xb0\xf7\x12\x06\x4a\x83\x06\x40\x88\x8f"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceFlatWabClose2)
{
    TestDraw drawable(800, 600);

    // FlatWabClose is a flat_wab_close widget of size 100x20 at position 10,100 in it's parent context
    WidgetScreen parent(drawable, 800, 600);
    NotifyApi * notifier = NULL;

    try {
    FlatWabClose flat_wab_close(drawable, 800, 600, parent, notifier,
        "Lorem ipsum dolor sit amet, consectetur<br>"
        "adipiscing elit. Nam purus lacus, luctus sit<br>"
        "amet suscipit vel, posuere quis turpis. Sed<br>"
        "venenatis rutrum sem ac posuere. Phasellus<br>"
        "feugiat dui eu mauris adipiscing sodales.<br>"
        "Mauris rutrum molestie purus, in tempor lacus<br>"
        "tincidunt et. Sed eu ligula mauris, a rutrum<br>"
        "est. Vestibulum in nunc vel massa condimentum<br>"
        "iaculis nec in arcu. Pellentesque accumsan,<br>"
        "quam sit amet aliquam mattis, odio purus<br>"
        "porttitor tortor, sit amet tincidunt odio<br>"
        "erat ut ligula. Fusce sit amet mauris neque.<br>"
        "Sed orci augue, luctus in ornare sed,<br>"
        "adipiscing et arcu."
    );

    flat_wab_close.rdp_input_invalidate(flat_wab_close.rect);
    }
    catch(Error & e) {
        LOG(LOG_INFO, "error.id=%d", e.id);
        BOOST_CHECK(false);
    };

    // ask to widget to redraw at it's current position

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close2.png");

    char message[1024];

    if (!check_sig(drawable.gd.drawable, message,
                   "\x4a\xb4\x8c\x03\x2a\xaf\x15\x92\x9c\xed"
                   "\x05\xd7\x2b\x6e\xc2\xfd\xcf\x5e\x20\xd7"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceFlatWabClose3)
{
    TestDraw drawable(800, 600);

    // FlatWabClose is a flat_wab_close widget of size 100x20 at position -10,500 in it's parent context
    WidgetScreen parent(drawable, 800, 600);
    NotifyApi * notifier = NULL;

    FlatWabClose flat_wab_close(drawable, 800, 600, parent, notifier,
                                    "abc<br>def");

    // ask to widget to redraw at it's current position
    flat_wab_close.rdp_input_invalidate(flat_wab_close.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close3.png");

    char message[1024];

    if (!check_sig(drawable.gd.drawable, message,
                   "\x87\xb8\xd7\x33\x1f\x29\x7e\xb1\x61\x05"
                   "\xb2\xb7\xaa\x1b\xaa\x71\x3c\x36\xf8\x94"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}




BOOST_AUTO_TEST_CASE(TraceFlatWabCloseClip)
{
    TestDraw drawable(800, 600);

    // FlatWabClose is a flat_wab_close widget of size 100x20 at position 760,-7 in it's parent context
    WidgetScreen parent(drawable, 800, 600);
    NotifyApi * notifier = NULL;

    FlatWabClose flat_wab_close(drawable, 800, 600, parent, notifier,
                                    "abc<br>def");

    // ask to widget to redraw at position 780,-7 and of size 120x20. After clip the size is of 20x13
    flat_wab_close.rdp_input_invalidate(flat_wab_close.rect.offset(20,0));

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close7.png");

    char message[1024];

    if (!check_sig(drawable.gd.drawable, message,
                   "\x47\xa6\x7e\x4b\xf7\x48\x3b\x94\xb0\x7f"
                   "\xad\x4f\xc4\xba\xeb\xbc\x11\x62\x77\x5e"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceFlatWabCloseClip2)
{
    TestDraw drawable(800, 600);

    // FlatWabClose is a flat_wab_close widget of size 100x20 at position 10,7 in it's parent context
    WidgetScreen parent(drawable, 800, 600);
    NotifyApi * notifier = NULL;

    FlatWabClose flat_wab_close(drawable, 800, 600, parent, notifier,
                                    "abc<br>def");

    // ask to widget to redraw at position 30,12 and of size 30x10.
    flat_wab_close.rdp_input_invalidate(Rect(20 + flat_wab_close.dx(),
                                               5 + flat_wab_close.dy(),
                                               30,
                                               10));

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close8.png");

    char message[1024];

    if (!check_sig(drawable.gd.drawable, message,
                   "\x3a\x57\x03\x57\x9a\x68\x07\xbd\x16\xd3"
                   "\x0d\xe1\x95\xf3\xf5\x0e\x1a\x9e\xec\xf9"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceFlatWabCloseExit)
{
    ClientInfo info(1, true, true);
    info.keylayout = 0x040C;
    info.console_session = 0;
    info.brush_cache_code = 0;
    info.bpp = 24;
    info.width = 800;
    info.height = 600;

    struct Notify : NotifyApi {
        Widget2* sender;
        notify_event_t event;

        Notify()
        : sender(0)
        , event(0)
        {}

        virtual void notify(Widget2* sender, notify_event_t event)
        {
            this->sender = sender;
            this->event = event;
        }
    } notifier;

    TestDraw drawable(800, 600);

    // FlatWabClose is a flat_wab_close widget of size 100x20 at position -10,500 in it's parent context
    WidgetScreen parent(drawable, 800, 600);

    FlatWabClose flat_wab_close(drawable, 800, 600, parent, &notifier,
                                "abc<br>def", 0, "tartempion", "caufield",
                                WHITE, DARK_BLUE_BIS, true);

    flat_wab_close.refresh_timeleft(183);

    // ask to widget to redraw at it's current position
    flat_wab_close.rdp_input_invalidate(flat_wab_close.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close-exit1.png");

    char message[1024];

    if (!check_sig(drawable.gd.drawable, message,
                   "\x50\x7f\x52\x59\x31\x72\x66\x4d\xed\x2b"
                   "\x1c\x24\x71\xf3\x01\xf0\x2e\x1b\x05\x10"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    flat_wab_close.refresh_timeleft(49);
    flat_wab_close.rdp_input_invalidate(flat_wab_close.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close-exit2.png");
    if (!check_sig(drawable.gd.drawable, message,
                   "\x0b\x7e\xc3\xa8\xcc\x91\x8f\x63\x0b\xea"
                   "\xdd\x75\x97\xd7\xe7\xe4\x88\x6a\xdf\x83"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    flat_wab_close.rdp_input_mouse(MOUSE_FLAG_BUTTON1|MOUSE_FLAG_DOWN,
                                   flat_wab_close.cancel.centerx(),
                                   flat_wab_close.cancel.centery(), NULL);
    flat_wab_close.rdp_input_mouse(MOUSE_FLAG_BUTTON1,
                                   flat_wab_close.cancel.centerx(),
                                   flat_wab_close.cancel.centery(), NULL);

    BOOST_CHECK(notifier.sender == &flat_wab_close);
    BOOST_CHECK(notifier.event == NOTIFY_CANCEL);

    // drawable.save_to_png(OUTPUT_FILE_PATH "flat_wab_close-exit3.png");
    if (!check_sig(drawable.gd.drawable, message,
                   "\x0b\x7e\xc3\xa8\xcc\x91\x8f\x63\x0b\xea"
                   "\xdd\x75\x97\xd7\xe7\xe4\x88\x6a\xdf\x83"
                   )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    notifier.sender = 0;
    notifier.event = 0;

    Keymap2 keymap;
    keymap.init_layout(info.keylayout);
    keymap.push_kevent(Keymap2::KEVENT_ESC);
    flat_wab_close.rdp_input_scancode(0, 0, 0, 0, &keymap);


}
