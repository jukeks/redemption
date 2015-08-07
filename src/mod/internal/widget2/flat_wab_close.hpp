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
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#if !defined(REDEMPTION_MOD_INTERNAL_WIDGET2_FLAT_WAB_CLOSE_HPP)
#define REDEMPTION_MOD_INTERNAL_WIDGET2_FLAT_WAB_CLOSE_HPP

#include "composite.hpp"
#include "flat_button.hpp"
#include "image.hpp"
#include "edit.hpp"
#include "label.hpp"
#include "multiline.hpp"
#include "translation.hpp"
#include "widget2_rect.hpp"
#include "theme.hpp"

#include <vector>

class FlatWabClose : public WidgetParent
{
public:
    int bg_color;

    WidgetImage img;
    WidgetLabel username_label;
    WidgetLabel username_label_value;
    WidgetLabel target_label;
    WidgetLabel target_label_value;
    WidgetLabel connection_closed_label;
    WidgetFlatButton cancel;
    WidgetLabel diagnostic;
    WidgetMultiLine diagnostic_lines;
    WidgetLabel timeleft_label;
    WidgetLabel timeleft_value;
    WidgetRect separator;

    CompositeArray composite_array;

private:
    long prev_time;

    Language lang;

public:
    FlatWabClose(DrawApi& drawable, int16_t width, int16_t height, Widget2& parent,
                 NotifyApi* notifier, const char * diagnostic_text, int group_id,
                 const char * username, const char * target,
                 bool showtimer, Font const & font, Theme const & theme, Language lang)
    : WidgetParent(drawable, Rect(0, 0, width, height), parent, notifier)
    , bg_color(theme.global.bgcolor)
    // , img(drawable, 0, 0, theme.global.logo_path, *this, nullptr, -10)
    , img(drawable, 0, 0,
          theme.global.logo ? theme.global.logo_path :
          SHARE_PATH "/" LOGIN_WAB_BLUE, *this, nullptr, -10)
    , username_label(drawable, (width - 600) / 2, 0, *this, nullptr, "Username:", true, -11,
                     theme.global.fgcolor, theme.global.bgcolor, font)
    , username_label_value(drawable, 0, 0, *this, nullptr, username, true, -11,
                           theme.global.fgcolor, theme.global.bgcolor, font)
    , target_label(drawable, (width - 600) / 2, 0, *this, nullptr, "Target:", true, -12,
                   theme.global.fgcolor, theme.global.bgcolor, font)
    , target_label_value(drawable, 0, 0, *this, nullptr, target, true, -12,
                         theme.global.fgcolor, theme.global.bgcolor, font)
    , connection_closed_label(drawable, 0, 0, *this, nullptr, TR("connection_closed", lang),
                              true, -13, theme.global.fgcolor,
                              theme.global.bgcolor, font)
    , cancel(drawable, 0, 0, *this, this, TR("close", lang), true, -14,
             theme.global.fgcolor, theme.global.bgcolor,
             theme.global.focus_color, font, 6, 2)
    , diagnostic(drawable, (width - 600) / 2, 0, *this, nullptr, "Diagnostic:", true, -15,
                 theme.global.fgcolor, theme.global.bgcolor, font)
    , diagnostic_lines(drawable, 0, 0, *this, nullptr, diagnostic_text, true, -16,
                       theme.global.fgcolor, theme.global.bgcolor, font)
    , timeleft_label(drawable, (width - 600) / 2, 0, *this, nullptr, "Time left:", true, -12,
                     theme.global.fgcolor, theme.global.bgcolor, font)
    , timeleft_value(drawable, 0, 0, *this, nullptr, nullptr, true, -12,
                     theme.global.fgcolor, theme.global.bgcolor, font)
    , separator(drawable, Rect(0, 0, width, 2), *this, this, -12,
                theme.global.separator_color)
    , prev_time(0)
    , lang(lang)
    {
        this->impl = &composite_array;

        char label[255];
        snprintf(label, sizeof(label), "%s:", TR("username", lang));
        this->username_label.set_text(label);
        snprintf(label, sizeof(label), "%s:", TR("target", lang));
        this->target_label.set_text(label);
        snprintf(label, sizeof(label), "%s:", TR("diagnostic", lang));
        this->diagnostic.set_text(label);
        snprintf(label, sizeof(label), "%s:", TR("timeleft", lang));
        this->timeleft_label.set_text(label);

        // this->img.rect.x = (this->cx() - this->img.cx()) / 2;
        this->cancel.set_button_x((this->cx() - this->cancel.cx()) / 2);
        this->connection_closed_label.rect.x = (this->cx() - this->connection_closed_label.cx()) / 2;

        this->separator.rect.x = (this->cx() - 600) / 2;
        this->separator.rect.cx = 600;

        this->add_widget(&this->connection_closed_label);
        this->add_widget(&this->cancel);
        this->add_widget(&this->diagnostic);
        this->add_widget(&this->diagnostic_lines);
        this->add_widget(&this->separator);

        uint16_t px = this->diagnostic.cx() + 10;

        int y = this->dy() + 10;
        // this->img.rect.y = y;
        // y += this->img.cy() + 20;

        this->connection_closed_label.rect.y = y;
        y += this->connection_closed_label.cy();

        this->separator.rect.y = y + 3;
        y += 30;

        if (username && *username) {
            this->add_widget(&this->username_label);
            this->add_widget(&this->username_label_value);
            this->add_widget(&this->target_label);
            this->add_widget(&this->target_label_value);

            px = std::max(this->username_label.cx(), this->diagnostic.cx());
            px = std::max(px, this->timeleft_label.cx()) + 10;
            this->username_label_value.rect.x = this->username_label.dx() + px;
            this->target_label_value.rect.x = this->username_label.dx() + px;

            this->username_label.rect.y = y;
            this->username_label_value.rect.y = y;
            y += this->username_label.cy() + 10;
            this->target_label.rect.y = y;
            this->target_label_value.rect.y = y;
            y += this->target_label.cy() + 20;
        }
        this->diagnostic.rect.y = y;
        if (this->diagnostic.cx() > this->cx() - (px + 10)) {
            y += this->diagnostic.cy() + 10;
            this->diagnostic_lines.rect.y = y;
            y += this->diagnostic_lines.cy() + 20;
        }
        else {
            this->diagnostic_lines.rect.y = y;
            y += std::max(this->diagnostic_lines.cy(), this->diagnostic.cy()) + 20;
            this->diagnostic_lines.rect.x = this->username_label.dx() + px;
        }

        if (showtimer) {
            this->add_widget(&this->timeleft_label);
            this->add_widget(&this->timeleft_value);
            this->timeleft_label.rect.y = y;
            this->timeleft_value.rect.y = y;
            if (this->timeleft_label.cx() + 10 > px) {
                px = this->timeleft_label.cx() + 10;
                this->diagnostic_lines.rect.x = this->username_label.dx() + px;
            }
            this->timeleft_value.rect.x =
                this->username_label.dx() + px;
            y += this->timeleft_label.cy() + 20;
        }

        this->cancel.set_button_y(y);
        y += this->cancel.cy();

        this->move_xy(0, (height - y) / 2);

        this->img.rect.x = (this->cx() - this->img.cx()) / 2;
        this->img.rect.y = (3*(height - y) / 2 - this->img.cy()) / 2 + y;
        this->add_widget(&this->img);

        this->set_widget_focus(&this->cancel, focus_reason_tabkey);
    }

    ~FlatWabClose() override {
        this->clear();
    }

    int get_bg_color() const override {
        return this->bg_color;
    }

    void refresh_timeleft(long tl) {
        bool seconds = true;
        if (tl > 60) {
            seconds = false;
            tl = tl / 60;
        }
        if (this->prev_time != tl) {
            char buff[256];
            snprintf(buff, sizeof(buff), "%2ld %s%s %s. ",
                     tl,
                     seconds?TR("second", this->lang):TR("minute", this->lang),
                     (tl <= 1)?"":"s",
                     TR("before_closing", this->lang)
                     );

            Rect old = this->timeleft_value.rect;
            this->drawable.begin_update();
            this->timeleft_value.set_text(nullptr);
            this->draw(old);
            this->timeleft_value.set_text(buff);
            this->draw(this->timeleft_value.rect);
            this->drawable.end_update();

            this->prev_time = tl;
        }
    }

    void notify(Widget2 * widget, NotifyApi::notify_event_t event) override {
        if (widget == &this->cancel && event == NOTIFY_SUBMIT) {
            this->send_notify(NOTIFY_CANCEL);
        }
        else {
            WidgetParent::notify(widget, event);
        }
    }

    void rdp_input_scancode(long int param1, long int param2, long int param3, long int param4, Keymap2* keymap) override {
        if (keymap->nb_kevent_available() > 0){
            switch (keymap->top_kevent()){
            case Keymap2::KEVENT_ESC:
                keymap->get_kevent();
                this->send_notify(NOTIFY_CANCEL);
                break;
            default:
                WidgetParent::rdp_input_scancode(param1, param2, param3, param4, keymap);
                break;
            }
        }
    }
};

#endif
