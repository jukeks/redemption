/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Xiaopeng Zhou, Jonathan Poelen
 */

#ifndef REDEMPTION_MOD_INTERNAL_WIDGET2_SELECTOR_MOD_HPP
#define REDEMPTION_MOD_INTERNAL_WIDGET2_SELECTOR_MOD_HPP

#include "front_api.hpp"
#include "config.hpp"
#include "selector.hpp"
#include "internal_mod.hpp"

class SelectorMod : public InternalMod, public NotifyApi
{
    WidgetSelector selector;
    int current_page;
    int number_page;

    char selector_current_page[16];
    char selector_number_of_pages[16];

    Inifile & ini;

public:
    SelectorMod(Inifile& ini, FrontAPI& front, uint16_t width, uint16_t height)
    : InternalMod(front, width, height)
    , selector(this, "bidule", width, height, this, NULL, NULL)
    , current_page(0)
    , number_page(0)
    , ini(ini)
    {
        this->refresh_context();

        //BEGIN TEST
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        this->selector.add_device("dsq", "dqfdfdfsfds", "fd", "fdsfsfd");
        //END TEST

        this->selector.refresh(this->selector.rect);
    }

    virtual ~SelectorMod()
    {}

    virtual void notify(Widget2* sender, notify_event_t event,
                        long unsigned int param, long unsigned int param2)
    {
        if (NOTIFY_SUBMIT == event) {
            if (sender == &this->selector.logout) {
                this->ini.context_ask(AUTHID_AUTH_USER);
                this->ini.context_ask(AUTHID_PASSWORD);
                this->ini.context_ask(AUTHID_TARGET_USER);
                this->ini.context_ask(AUTHID_TARGET_DEVICE);
                this->ini.context_ask(AUTHID_SELECTOR);
                this->signal = BACK_EVENT_NEXT;
                this->event.set();
            }
            else if (sender == &this->selector.connect) {
            }
            else if (sender == &this->selector.apply
                || sender == &this->selector.filter_account_device
                || sender == &this->selector.filter_device_group
            ) {
            }
            else if (sender == &this->selector.first_page) {
                if (this->current_page > 1) {
                    this->current_page = 1;
                }
            }
            else if (sender == &this->selector.prev_page) {
                if (this->current_page > 0) {
                    --this->current_page;
                }
            }
            else if (sender == &this->selector.current_page) {
                int page = atoi(this->selector.current_page.label.buffer);
                if (page != this->current_page) {
                    this->current_page = page;
                }
            }
            else if (sender == &this->selector.next_page) {
                if (this->current_page < this->number_page) {
                    ++this->current_page;
                }
            }
            else if (sender == &this->selector.last_page) {
                if (this->current_page < this->number_page) {
                    this->current_page = this->number_page;
                }
            }
        }
    }

    void refresh_context()
    {
        const char * s = this->ini.context_get_value(AUTHID_SELECTOR_DEVICE_FILTER, NULL, 0);
        if (*s) {
            this->selector.filter_device_group.label.set_text(s);
        }

        s = this->ini.context_get_value(AUTHID_SELECTOR_GROUP_FILTER, NULL, 0);
        if (*s) {
            this->selector.filter_account_device.label.set_text(s);
        }

        s = ini.context_get_value(AUTHID_SELECTOR_CURRENT_PAGE, this->selector_current_page, sizeof(this->selector_current_page));
        this->selector.current_page.label.set_text(s);
        this->current_page = atoi(s);

        s = ini.context_get_value(AUTHID_SELECTOR_NUMBER_OF_PAGES, this->selector_number_of_pages, sizeof(this->selector_number_of_pages));
        this->selector.number_page.set_text(WidgetSelector::temporary_number_of_page(s).buffer);
        this->number_page = atoi(s);

        char * groups    = const_cast<char *>(this->ini.context_get_value(AUTHID_TARGET_USER, NULL, 0));
        char * targets   = const_cast<char *>(this->ini.context_get_value(AUTHID_TARGET_DEVICE, NULL, 0));
        char * protocols = const_cast<char *>(this->ini.context_get_value(AUTHID_TARGET_PROTOCOL, NULL, 0));
        char * endtimes  = const_cast<char *>((const char *)this->ini.globals.context.end_time);

        for (size_t index = 0 ; index < 50 ; index++) {
            size_t size_groups = proceed_item(groups);
            size_t size_targets = proceed_item(targets);
            size_t size_protocols = proceed_item(protocols);
            size_t size_endtimes = proceed_item(endtimes, ';');

            char c_group = groups[size_groups];
            char c_target = targets[size_targets];
            char c_protocol = protocols[size_protocols];
            char c_endtime = endtimes[size_endtimes];

            groups[size_groups] = '\0';
            targets[size_targets] = '\0';
            protocols[size_protocols] = '\0';
            endtimes[size_endtimes] = '\0';

            this->selector.add_device(groups, targets, protocols, endtimes);

            groups[size_groups] = c_group;
            targets[size_targets] = c_target;
            protocols[size_protocols] = c_protocol;
            endtimes[size_endtimes] = c_endtime;

            if (c_group    == '\n' || !c_group
            ||  c_target   == '\n' || !c_target
            ||  c_protocol == '\n' || !c_protocol
            ||  c_endtime  == '\n' || !c_endtime
            ){
                break;
            }

            groups += size_groups + 1;
            targets += size_targets + 1;
            protocols += size_protocols + 1;
            endtimes += size_endtimes + 1;
        }

        this->selector.account_device_lines.init_current_index(0);
        this->selector.device_group_lines.init_current_index(0);
        this->selector.close_time_lines.init_current_index(0);
        this->selector.protocol_lines.init_current_index(0);
    }

    static inline size_t proceed_item(const char * list, char sep = ' ')
    {
        const char * p = list;
        while (*p != sep && *p != '\n' && *p){
            p++;
        }
        return p - list;
    }

    virtual void rdp_input_invalidate(const Rect& r)
    {
        this->selector.rdp_input_invalidate(r);
    }

    virtual void rdp_input_mouse(int device_flags, int x, int y, Keymap2* keymap)
    {
        this->selector.rdp_input_mouse(device_flags, x, y, keymap);
    }

    virtual void rdp_input_scancode(long int param1, long int param2, long int param3, long int param4, Keymap2* keymap)
    {
        this->selector.rdp_input_scancode(param1, param2, param3, param4, keymap);
    }

    virtual void rdp_input_synchronize(uint32_t time, uint16_t device_flags, int16_t param1, int16_t param2)
    {}

    virtual BackEvent_t draw_event()
    {
        this->event.reset();
        return this->signal;
    }

    virtual void server_draw_text(int16_t x, int16_t y, const char * text, uint32_t fgcolor, uint32_t bgcolor, const Rect & clip)
    {
        TODO("bgcolor <-> fgcolor")
        this->front.server_draw_text(x, y, text, bgcolor, fgcolor, clip);
    }
};

#endif