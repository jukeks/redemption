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
   Author(s): Christophe Grosjean, Clément Moroldo

   Fake Front class for Unit Testing
*/

#ifndef FRONT_QT_HPP
#define FRONT_QT_HPP

#include <stdio.h>
#include <openssl/ssl.h>
#include <iostream>
#include <stdint.h>

#include "RDP/caches/brushcache.hpp"
#include "RDP/capabilities/colcache.hpp"
#include "RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"
#include "RDP/orders/RDPOrdersPrimaryEllipseCB.hpp"
#include "RDP/orders/RDPOrdersPrimaryScrBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryMultiDstBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryMultiOpaqueRect.hpp"
#include "RDP/orders/RDPOrdersPrimaryDestBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryMultiPatBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryMultiScrBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryPatBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryMemBlt.hpp"
#include "RDP/orders/RDPOrdersPrimaryMem3Blt.hpp"
#include "RDP/orders/RDPOrdersPrimaryLineTo.hpp"
#include "RDP/orders/RDPOrdersPrimaryGlyphIndex.hpp"
#include "RDP/orders/RDPOrdersPrimaryPolyline.hpp"
#include "RDP/orders/RDPOrdersPrimaryPolygonCB.hpp"
#include "RDP/orders/RDPOrdersPrimaryPolygonSC.hpp"
#include "RDP/orders/RDPOrdersSecondaryFrameMarker.hpp"
#include "RDP/orders/RDPOrdersPrimaryEllipseSC.hpp"
#include "RDP/orders/RDPOrdersSecondaryGlyphCache.hpp"
#include "RDP/orders/AlternateSecondaryWindowing.hpp"

#include "log.hpp"
#include "front_api.hpp"
#include "channel_list.hpp"
#include "client_info.hpp"
#include "mod_api.hpp"
#include "bitmap.hpp"
#include "keymap2.hpp"
#include "RDP/caches/glyphcache.hpp"
#include "RDP/capabilities/glyphcache.hpp"
#include "RDP/bitmapupdate.hpp"


#include <QtGui/QWidget>
#include <QtGui/QPicture>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>
#include <QtCore/QSocketNotifier>



class Front_Qt : public QWidget, public FrontAPI
{
    
Q_OBJECT
    
    
public:
    uint32_t                    verbose;
    ClientInfo                & info;
    CHANNELS::ChannelDefArray   cl;
    
    // Graphic members
    uint8_t               mod_bpp;
    BGRPalette            mod_palette;
    int                   mouse_x;
    int                   mouse_y;
    bool                  notimestamp;
    bool                  nomouse;
    QLabel               _label;
    QPicture             _picture;
    int                  _width;
    int                  _height;
    QPen                 _pen;
    QPainter             _painter;
    
    // Connexion socket members
    QSocketNotifier      _sckRead;
    mod_api*             _callback;
    
    // Controllers members
    Keymap2                        _keymap;
    bool                           _capslock;
    bool                           _shift;
    bool                           _altgr;
    bool                           _ctrl;
    const Keylayout::KeyLayout_t * _layout;
    bool                           _ctrl_alt_delete;
    StaticOutStream<256>           _decoded_data;
    uint8_t                        _keyboardMods;
    
    enum {
          CTRL_MOD     = 0x08
        , CAPSLOCK_MOD = 0x04
        , ALT_MOD      = 0x02
        , SHIFT_MOD    = 0x01
    };
     

    QColor u32_to_qcolor(uint32_t color){
        uint8_t b(color >> 16);
        uint8_t g(color >> 8);
        uint8_t r(color);
        return {r, g, b};
    }
    
    void reInitView() {
        this->_painter.begin(&(this->_picture));
        this->_painter.fillRect(0, 0, this->_width, this->_height, QColor(0, 0, 0, 0));
    }
    
    virtual void flush() override {
        if (this->verbose > 10) {
             LOG(LOG_INFO, "--------- FRONT ------------------------");
             LOG(LOG_INFO, "flush()");
             LOG(LOG_INFO, "========================================\n");
        }
        this->_painter.end();
        this->_label.setPicture(this->_picture);
        this->show();
    }
    
    void refresh() {
        Rect rect(0, 0, this->_width, this->_height);
        this->_callback->rdp_input_invalidate(rect);
    }
    
    virtual const CHANNELS::ChannelDefArray & get_channel_list(void) const override { return cl; }

    virtual void send_to_channel( const CHANNELS::ChannelDef & channel, uint8_t const * data, size_t length
                                , size_t chunk_size, int flags) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            LOG(LOG_INFO, "send_to_channel");
            LOG(LOG_INFO, "========================================\n");
        }
    }

    virtual void send_global_palette() override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            LOG(LOG_INFO, "send_global_palette()");
            LOG(LOG_INFO, "========================================\n");
        }
    }

    virtual void begin_update() override {
        //if (this->verbose > 10) {
        //    LOG(LOG_INFO, "--------- FRONT ------------------------");
        //    LOG(LOG_INFO, "begin_update");
        //    LOG(LOG_INFO, "========================================\n");
        //}
    }

    virtual void end_update() override {
        //if (this->verbose > 10) {
        //    LOG(LOG_INFO, "--------- FRONT ------------------------");
        //    LOG(LOG_INFO, "end_update");
        //    LOG(LOG_INFO, "========================================\n");
        //}
    }

    virtual void set_mod_palette(const BGRPalette & palette) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            LOG(LOG_INFO, "set_mod_palette");
            LOG(LOG_INFO, "========================================\n");
        }
    }

    virtual void server_set_pointer(const Pointer & cursor) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            LOG(LOG_INFO, "server_set_pointer");
            LOG(LOG_INFO, "========================================\n");
        }

        //this->gd.server_set_pointer(cursor);
    }

    virtual int server_resize(int width, int height, int bpp) override {
        this->mod_bpp = bpp;
        this->info.bpp = bpp;
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            LOG(LOG_INFO, "server_resize(width=%d, height=%d, bpp=%d", width, height, bpp);
            LOG(LOG_INFO, "========================================\n");
        }
        return 1;
    }
    
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
    //-------------------------------
    //      DRAWING FUNCTIONS 
    //------------------------------

    virtual void draw(const RDPOpaqueRect & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        //std::cout << "RDPOpaqueRect" << std::endl;

        RDPOpaqueRect new_cmd24 = cmd;
        new_cmd24.color = color_decode_opaquerect(cmd.color, this->mod_bpp, this->mod_palette);
        
        Rect rect(new_cmd24.rect.intersect(clip));
        this->_painter.fillRect(rect.x, rect.y, rect.cx, rect.cy, this->u32_to_qcolor(new_cmd24.color));
    }

    virtual void draw(const RDPScrBlt & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPScrBlt" << std::endl;
    }

    virtual void draw(const RDPDestBlt & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPDestBlt" << std::endl;
    }

    virtual void draw(const RDPMultiDstBlt & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPMultiDstBlt" << std::endl;
    }

    virtual void draw(const RDPMultiOpaqueRect & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPMultiOpaqueRect" << std::endl;
    }

    virtual void draw(const RDP::RDPMultiPatBlt & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPMultiPatBlt" << std::endl;
    }

    virtual void draw(const RDP::RDPMultiScrBlt & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPMultiScrBlt" << std::endl;
    }

    virtual void draw(const RDPPatBlt & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        //std::cout << "RDPPatBlt" << std::endl;

        /*RDPPatBlt new_cmd24 = cmd;
        new_cmd24.back_color = color_decode_opaquerect(cmd.back_color, this->mod_bpp, this->mod_palette);
        new_cmd24.fore_color = color_decode_opaquerect(cmd.fore_color, this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip);
    }

    virtual void draw(const RDPMemBlt & cmd, const Rect & clip, const Bitmap & bitmap) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPMemBlt" << std::endl;
    }

    virtual void draw(const RDPMem3Blt & cmd, const Rect & clip, const Bitmap & bitmap) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }

        std::cout << "RDPMem3Blt" << std::endl;
    }

    virtual void draw(const RDPLineTo & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPLineTo" << std::endl;
        
        RDPLineTo new_cmd24 = cmd;
        new_cmd24.back_color = color_decode_opaquerect(cmd.back_color, this->mod_bpp, this->mod_palette);
        new_cmd24.pen.color  = color_decode_opaquerect(cmd.pen.color,  this->mod_bpp, this->mod_palette);

        
        // TO DO clipping
        this->_pen.setBrush(this->u32_to_qcolor(new_cmd24.back_color));
        this->_painter.drawLine(new_cmd24.startx, new_cmd24.starty, new_cmd24.endx, new_cmd24.endy);
    }

    virtual void draw(const RDPGlyphIndex & cmd, const Rect & clip, const GlyphCache * gly_cache) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPGlyphIndex" << std::endl;

       /* RDPGlyphIndex new_cmd24 = cmd;
        new_cmd24.back_color = color_decode_opaquerect(cmd.back_color, this->mod_bpp, this->mod_palette);
        new_cmd24.fore_color = color_decode_opaquerect(cmd.fore_color, this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip, gly_cache);
    }

    void draw(const RDPPolygonSC & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPPolygonSC" << std::endl;

        /*RDPPolygonSC new_cmd24 = cmd;
        new_cmd24.BrushColor  = color_decode_opaquerect(cmd.BrushColor,  this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip);
    }

    void draw(const RDPPolygonCB & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPPolygonCB" << std::endl;

        /*RDPPolygonCB new_cmd24 = cmd;
        new_cmd24.foreColor  = color_decode_opaquerect(cmd.foreColor,  this->mod_bpp, this->mod_palette);
        new_cmd24.backColor  = color_decode_opaquerect(cmd.backColor,  this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip);
    }

    void draw(const RDPPolyline & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPPolyline" << std::endl;

        /*RDPPolyline new_cmd24 = cmd;
        new_cmd24.PenColor  = color_decode_opaquerect(cmd.PenColor,  this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip);
    }

    virtual void draw(const RDPEllipseSC & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPEllipseSC" << std::endl;

        /*RDPEllipseSC new_cmd24 = cmd;
        new_cmd24.color = color_decode_opaquerect(cmd.color, this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip);
    }

    virtual void draw(const RDPEllipseCB & cmd, const Rect & clip) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            cmd.log(LOG_INFO, clip);
            LOG(LOG_INFO, "========================================\n");
        }
        
        std::cout << "RDPEllipseCB" << std::endl;
/*
        RDPEllipseCB new_cmd24 = cmd;
        new_cmd24.fore_color = color_decode_opaquerect(cmd.fore_color, this->mod_bpp, this->mod_palette);
        new_cmd24.back_color = color_decode_opaquerect(cmd.back_color, this->mod_bpp, this->mod_palette);*/
        //this->gd.draw(new_cmd24, clip);
    }

    virtual void draw(const RDP::FrameMarker & order) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            order.log(LOG_INFO);
            LOG(LOG_INFO, "========================================\n");
        }

        //this->gd.draw(order);
    }

    virtual void draw(const RDP::RAIL::NewOrExistingWindow & order) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            order.log(LOG_INFO);
            LOG(LOG_INFO, "========================================\n");
        }

        //this->gd.draw(order);
    }

    virtual void draw(const RDP::RAIL::WindowIcon & order) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            order.log(LOG_INFO);
            LOG(LOG_INFO, "========================================\n");
        }

        //this->gd.draw(order);
    }

    virtual void draw(const RDP::RAIL::CachedIcon & order) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            order.log(LOG_INFO);
            LOG(LOG_INFO, "========================================\n");
        }

        //this->gd.draw(order);
    }

    virtual void draw(const RDP::RAIL::DeletedWindow & order) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            order.log(LOG_INFO);
            LOG(LOG_INFO, "========================================\n");
        }

        //this->gd.draw(order);
    }
    
    virtual void draw(const RDPColCache   & cmd) {}
    
    virtual void draw(const RDPBrushCache & cmd) {}
    
    
    void draw(const RDPBitmapData & bitmap_data, const uint8_t * data,
        size_t size, const Bitmap & bmp) override {
        if (this->verbose > 10) {
            LOG(LOG_INFO, "--------- FRONT ------------------------");
            bitmap_data.log(LOG_INFO, "FakeFront");
            LOG(LOG_INFO, "========================================\n");
        }
        
        //std::cout << "RDPBitmapData" << std::endl;
        if (!bmp.is_valid()){
            return;
        }

        const QRect rectBmp( bitmap_data.dest_left, bitmap_data.dest_top, 
                             (bitmap_data.dest_right - bitmap_data.dest_left + 1), 
                             (bitmap_data.dest_bottom - bitmap_data.dest_top + 1));
        const QRect clipRect(0, 0, this->_width, this->_height);
        const QRect rect = rectBmp.intersected(clipRect);
            
        const int16_t mincx = std::min<int16_t>(bmp.cx(), std::min<int16_t>(this->_width - rect.x(), rect.width()));
        const int16_t mincy = 1;

        if (mincx <= 0 || mincy <= 0) {
            return;
        }        
        
        int rowYCoord(rect.y() + rect.height()-1);
        int rowsize(bmp.line_size()); //Bpp
      
        const uint8_t * row = bmp.data();
        
        QImage::Format format; //bpp
        if (bmp.bpp() == 16){
            format = QImage::Format_RGB16;
        }
        if (bmp.bpp() == 24){
            format = QImage::Format_RGB888;
        }
        if (bmp.bpp() == 32){
            format = QImage::Format_RGB32;
        }
        if (bmp.bpp() == 15){
            format = QImage::Format_RGB555;
        }
        
        for (size_t k = 0 ; k < bitmap_data.height; k++) {
            
            QImage qbitmap(const_cast<unsigned char*>(row), mincx, mincy, format);
            const QRect trect(rect.x(), rowYCoord, mincx, mincy);
            this->_painter.drawImage(trect, qbitmap);

            row += rowsize;
            rowYCoord--;
        }
    }
    
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
    //------------------------
    //      CONSTRUCTOR
    //------------------------
 
    Front_Qt(ClientInfo & info, uint32_t verbose, int client_sck)
    : QWidget(), FrontAPI(false, false)
    , verbose(verbose)
    , info(info)
    , mod_bpp(info.bpp)
    , mod_palette(BGRPalette::no_init())
    , mouse_x(0)
    , mouse_y(0)
    , notimestamp(true)
    , nomouse(true) 
    , _label(this)
    , _picture()
    , _width(info.width)
    , _height(info.height)
    , _pen() 
    , _painter()
    , _sckRead(client_sck, QSocketNotifier::Read, this)
    , _keymap() 
    , _capslock(false)
    , _shift(false)
    , _altgr(false)
    , _ctrl(false)
    , _keyboardMods(0) {
        if (this->mod_bpp == 8) {
            this->mod_palette = BGRPalette::classic_332();
        }
        
        _keymap.init_layout(info.keylayout);
        this->_layout = &(this->_keymap.keylayout_WORK->noMod);
            
        this->setFixedSize(_width, _height);
            
        QSize size(sizeHint());
        QDesktopWidget* desktop = QApplication::desktop();
        int centerW = (desktop->width()/2)  - (size.width()/2);
        int centerH = (desktop->height()/2) - (size.height()/2);
        this->move(centerW, centerH);
            
        this->_label.setMouseTracking(true);
        this->_label.installEventFilter(this);
            
        this->_painter.setRenderHint(QPainter::Antialiasing);
        this->_painter.fillRect(0, 0, this->_width, this->_height, Qt::white);
        this->_pen.setWidth(1);
        this->_painter.setPen(this->_pen);
            
        this->setAttribute(Qt::WA_NoSystemBackground);


        // -------- Start of system wide SSL_Ctx option ------------------------------

        // ERR_load_crypto_strings() registers the error strings for all libcrypto
        // functions. SSL_load_error_strings() does the same, but also registers the
        // libssl error strings.

        // One of these functions should be called before generating textual error
        // messages. However, this is not required when memory usage is an issue.

        // ERR_free_strings() frees all previously loaded error strings.

        //SSL_load_error_strings();

        // SSL_library_init() registers the available SSL/TLS ciphers and digests.
        // OpenSSL_add_ssl_algorithms() and SSLeay_add_ssl_algorithms() are synonyms
        // for SSL_library_init().

        // - SSL_library_init() must be called before any other action takes place.
        // - SSL_library_init() is not reentrant.
        // - SSL_library_init() always returns "1", so it is safe to discard the return
        // value.

        // Note: OpenSSL 0.9.8o and 1.0.0a and later added SHA2 algorithms to
        // SSL_library_init(). Applications which need to use SHA2 in earlier versions
        // of OpenSSL should call OpenSSL_add_all_algorithms() as well.

        //SSL_library_init();
    }
    
    ~Front_Qt() {}
    
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
    //------------------------
    //      CONTROLLERS
    //------------------------
    
    void layout_Work_Update() {
        //                                ___________________________
        //                               |      |      |      |      |
        //   bitcode for _keyboardMods:  | CTRL | CAPS | ALT  | SHFT |
        //                               |______|______|______|______|
        
        if (this->_keyboardMods & CTRL_MOD) {
            this->_layout = &(this->_keymap.keylayout_WORK->ctrl);
        } else {
            switch (this->_keyboardMods) {
                
                case 0 : this->_layout = &(this->_keymap.keylayout_WORK->noMod);               break;
                case 1 : this->_layout = &(this->_keymap.keylayout_WORK->capslock_shift);      break;
                case 2 : this->_layout = &(this->_keymap.keylayout_WORK->altGr);               break;
                case 3 : this->_layout = &(this->_keymap.keylayout_WORK->shiftAltGr);          break;
                case 4 : this->_layout = &(this->_keymap.keylayout_WORK->capslock_noMod);      break;
                case 5 : this->_layout = &(this->_keymap.keylayout_WORK->shift);               break;
                case 6 : this->_layout = &(this->_keymap.keylayout_WORK->capslock_altGr);      break;
                case 7 : this->_layout = &(this->_keymap.keylayout_WORK->capslock_shiftAltGr); break;
                
                default: break;
            }   
        }
    }
    
    void keyQtEvent(int keyStatusFlag, QKeyEvent *e) { 
        int keyCode(e->key()); 
        bool unrecognised(false);
        if (keyCode != 0) {
            
            if (keyCode < 255 && keyCode > 0) {
                keyCode = e->text().toStdString()[0];
                int i(0);
                if (this->_layout != nullptr) {
                    const Keylayout::KeyLayout_t & layout = *(this->_layout);
                    for (; i < 128 && layout[i] != keyCode; i++) {}; //std::cout << "layoutVal=" << layout[i] << " i=" << i << " keycode=" << keyCode << std::endl;}
                }
                keyCode = i;
                
                this->_keymap.event(0x0000, keyCode, this->_decoded_data, this->_ctrl_alt_delete); 
                this->_callback->rdp_input_scancode(keyCode, 0, keyStatusFlag, 0000, &(this->_keymap)); std::cout << "keyPressed " << e->key() << " " << keyCode << std::endl;
                
            } else {
                int keyboardFlag(0);
                
                switch (keyCode) {
                    
                    //--------------
                    // keyboard mod
                    //--------------
                    case 16777251 : keyCode = 0x38;   //  L ALT
                        keyboardFlag = 0x0100;
                        if (keyStatusFlag == 0x0000) {
                            this->_altgr = true;
                            if (!this->_ctrl) {
                                if (this->_shift && this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_shiftAltGr);
                                }
                                if (!this->_shift && this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_altGr);
                                }
                                if (this->_shift && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->shiftAltGr);
                                } 
                                if (!this->_shift && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->altGr);
                                }
                            }
                            
                        } else {
                            this->_altgr = false;
                            if (this->_ctrl) {
                                this->_layout = &(this->_keymap.keylayout_WORK->ctrl);
                            } else {
                                if (this->_shift && this->_capslock)   {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_shift);
                                }
                                if (!this->_shift && this->_capslock)  {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_noMod);
                                }
                                if (this->_shift && !this->_capslock)  {
                                    this->_layout = &(this->_keymap.keylayout_WORK->shift);
                                }
                                if (!this->_shift && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->noMod);                      
                                }
                            }
                        }
                        break; 
                        
                    case 16781571 : keyCode = 0x38;   //  R ALT GR
                        keyboardFlag = 0x0100;
                        if (keyStatusFlag == 0x0000) {
                            this->_altgr = true;
                            if (!this->_ctrl) {
                                if (this->_shift && this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_shiftAltGr);
                                }
                                if (!this->_shift && this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_altGr);
                                }
                                if (this->_shift && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->shiftAltGr);
                                } 
                                if (!this->_shift && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->altGr);
                                }
                            }
                            
                        } else {
                            this->_altgr = false;
                            if (this->_ctrl) {
                                this->_layout = &(this->_keymap.keylayout_WORK->ctrl);
                            } else {
                                if (this->_shift && this->_capslock)   {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_shift);
                                }
                                if (!this->_shift && this->_capslock)  {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_noMod);   
                                }
                                if (this->_shift && !this->_capslock)  {
                                    this->_layout = &(this->_keymap.keylayout_WORK->shift);
                                }
                                if (!this->_shift && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->noMod);
                                }
                            }
                        }
                        break; 
                        
                    case 16777249 : keyCode = 0x1D;   //  R L CTRL
                        keyboardFlag = 0x0100;
                        if (keyStatusFlag == 0x0000) {
                            this->_ctrl = true;
                            if (this->_altgr) {
                                this->_layout = &(this->_keymap.keylayout_WORK->altGr);
                            } else {
                                this->_layout = &(this->_keymap.keylayout_WORK->ctrl);
                            } 
                            
                        } else {
                            this->_ctrl = false;
                            if (this->_shift && this->_altgr && this->_capslock)   {
                                this->_layout = &(this->_keymap.keylayout_WORK->capslock_shiftAltGr);
                            }
                            if (!this->_shift && this->_altgr  && this->_capslock) {
                                this->_layout = &(this->_keymap.keylayout_WORK->capslock_altGr);
                            }
                            if (this->_shift && !this->_altgr && this->_capslock)  {
                                this->_layout = &(this->_keymap.keylayout_WORK->capslock_shift);   
                            }
                            if (!this->_shift && !this->_altgr && this->_capslock) {
                                this->_layout = &(this->_keymap.keylayout_WORK->capslock_noMod);
                            }
                            if (this->_shift && this->_altgr && !this->_capslock)   {
                                this->_layout = &(this->_keymap.keylayout_WORK->shiftAltGr);
                            }
                            if (!this->_shift && this->_altgr && !this->_capslock)  {
                                this->_layout = &(this->_keymap.keylayout_WORK->altGr);
                            }
                            if (this->_shift && !this->_altgr && !this->_capslock)  {
                                this->_layout = &(this->_keymap.keylayout_WORK->shift);
                            }
                            if (!this->_shift && !this->_altgr && !this->_capslock) {
                                this->_layout = &(this->_keymap.keylayout_WORK->noMod);
                            }
                        }
                        break; 
                        
                    case 16777248 : keyCode = 0x36;   // R L SHFT
                        if (keyStatusFlag == 0x0000) {
                            this->_shift = true;
                            if (!this->_ctrl) {
                                if (this->_altgr && this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_shiftAltGr);
                                }
                                if (!this->_altgr && this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_shift);
                                }
                                if (this->_altgr && !this->_capslock) {                                
                                    this->_layout = &(this->_keymap.keylayout_WORK->shiftAltGr);
                                } 
                                if (!this->_altgr && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->shift);
                                }
                            }
                            
                        } else {
                            this->_shift = false;
                            if (this->_ctrl) {
                                this->_layout = &(this->_keymap.keylayout_WORK->ctrl);
                            } else {
                                if (this->_altgr && this->_capslock)   {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_altGr);
                                }
                                if (!this->_altgr && this->_capslock)  {
                                    this->_layout = &(this->_keymap.keylayout_WORK->capslock_noMod);
                                }
                                if (this->_altgr && !this->_capslock)  {
                                    this->_layout = &(this->_keymap.keylayout_WORK->altGr);
                                }
                                if (!this->_altgr && !this->_capslock) {
                                    this->_layout = &(this->_keymap.keylayout_WORK->noMod);
                                }
                            }
                        }
                        break; 
                        
                    case 16777252 : keyCode = 0x3A;   //  CAPSLOCK 
                        if (keyStatusFlag == 0x0000) {
                            if (!this->_capslock) {
                                this->_capslock = true;
                                if (!this->_ctrl) {
                                    if (this->_shift && this->_altgr)   {
                                        this->_layout = &(this->_keymap.keylayout_WORK->capslock_shiftAltGr);
                                    }
                                    if (!this->_shift && this->_altgr)  {
                                        this->_layout = &(this->_keymap.keylayout_WORK->capslock_altGr);
                                    }
                                    if (this->_shift && !this->_altgr)  {
                                        this->_layout = &(this->_keymap.keylayout_WORK->capslock_shift);
                                    }
                                    if (!this->_shift && !this->_altgr) {
                                        this->_layout = &(this->_keymap.keylayout_WORK->capslock_noMod);
                                    }
                                }
                                
                            } else {
                                if (!this->_ctrl) {
                                    this->_capslock = false;
                                    if (this->_shift && this->_altgr)   {
                                        this->_layout = &(this->_keymap.keylayout_WORK->shiftAltGr);
                                    }
                                    if (!this->_shift && this->_altgr)  {
                                        this->_layout = &(this->_keymap.keylayout_WORK->altGr);
                                    }
                                    if (this->_shift && !this->_altgr)  {
                                        this->_layout = &(this->_keymap.keylayout_WORK->shift);
                                    }
                                    if (!this->_shift && !this->_altgr) {
                                        this->_layout = &(this->_keymap.keylayout_WORK->noMod);
                                    }
                                }
                            }
                        }
                        break;
                     
                    //----------------------------
                    // None mod neither char keys
                    //----------------------------
                    case 16777221 : keyCode = 0x1C; break; //  ENTER KP
                    case 16777220 : keyCode = 0x1C; break; //  ENTER
                    case 16777219 : keyCode = 0x0E; break; //  BKSP
                    case 16777216 : keyCode = 0x01; break; //  ESCAPE
                    case 16777222 : keyCode = 0x52; keyboardFlag = 0x0100; break; //  INSERT
                    case 16777223 : keyCode = 0x53; keyboardFlag = 0x0100; break; //  DELETE               
                    case 16777233 : keyCode = 0x4F; keyboardFlag = 0x0100; break; //  END
                    case 16777239 : keyCode = 0x51; keyboardFlag = 0x0100; break; //  PG DN
                    case 16777238 : keyCode = 0x49; keyboardFlag = 0x0100; break; //  PG UP
                    case 16777235 : keyCode = 0x48; keyboardFlag = 0x0100; break; //  U ARROW
                    case 16777234 : keyCode = 0x4B; keyboardFlag = 0x0100; break; //  L ARROW
                    case 16777237 : keyCode = 0x50; keyboardFlag = 0x0100; break; //  D ARROW
                    case 16777236 : keyCode = 0x4D; keyboardFlag = 0x0100; break; //  R ARROW
                    case 16777264 : keyCode = 0x3B; break; //  F1
                    case 16777265 : keyCode = 0x3C; break; //  F2
                    case 16777266 : keyCode = 0x3D; break; //  F3
                    case 16777267 : keyCode = 0x3E; break; //  F4
                    case 16777268 : keyCode = 0x3F; break; //  F5
                    case 16777269 : keyCode = 0x40; break; //  F6
                    case 16777270 : keyCode = 0x41; break; //  F7
                    case 16777271 : keyCode = 0x42; break; //  F8
                    case 16777272 : keyCode = 0x43; break; //  F9
                    case 16777273 : keyCode = 0x44; break; //  F10
                    case 16777274 : keyCode = 0x57; break; //  F11
                    case 16777275 : keyCode = 0x58; break; //  F12
                    case 16777254 : keyCode = 0x46; break; //  SCROLL
                    case 16777224 : keyCode = 0xE1; break; //  PAUSE
                    case 16777217 : keyCode = 0x0F; break; //  TAB
                    case 338      : keyCode = 0x29; break; //  œ
                    
                    // 16781906 // trema + circonflex
                    //case 36 : scanCode = 0xE0; break; //  HOME
                    //case 0  : keyCode = 0xE0; break; //  PRNT_SCRN
                    //case 16777232 : keyCode = 0x00; break; // arrow between insert and up pg        
                    //case 0  : keyCode = 0xE0; break; //  R GUI
                    //case 0  : keyCode = 0xE0; break; //  APPS
                    //case 0  : keyCode = 0xE0; break; //  L GUI
                    //case 144: keyCode = 0x45; break; //  NUM
                    //case 16777301 : keyCOde = 0x00; break; // key beside R CTRL
                    //case 16777250 : keyCOde = 0x00; break; // R window
                    
                    default: 
                        unrecognised = true;
                        break;
                }
                
                if (!unrecognised) {
                    this->_keymap.event(keyStatusFlag | keyboardFlag, keyCode, this->_decoded_data, this->_ctrl_alt_delete); 
                    this->_callback->rdp_input_scancode(keyCode, 0, keyStatusFlag | keyboardFlag, 0000, &(this->_keymap)); 
                }
            } 
            std::cout << "keyPressed " << e->key() << " " << keyCode << std::endl;
        }
    }
    
    void mousePressEvent(QMouseEvent *e) {
        int flag(0); //std::cout << "click   " << "x=" << e->x() << " y=" << e->y() << " button:" << e->button() << std::endl;
        switch (e->button()) {
            case 1: flag = MOUSE_FLAG_BUTTON1; break;
            case 2: flag = MOUSE_FLAG_BUTTON2; break; 
            case 4: flag = MOUSE_FLAG_BUTTON4; break;
            default: break;
        }
        this->_callback->rdp_input_mouse(flag | MOUSE_FLAG_DOWN, e->x(), e->y(), &(this->_keymap));
    }
    
    void mouseReleaseEvent(QMouseEvent *e) {
        int flag(0); //std::cout << "release " << "x=" << e->x() << " y=" << e->y() << " button:" << e->button() << std::endl;
        switch (e->button()) {
            case 1: flag = MOUSE_FLAG_BUTTON1; break; 
            case 2: flag = MOUSE_FLAG_BUTTON2; break; 
            case 4: flag = MOUSE_FLAG_BUTTON4; break; 
            default: break;
        }
        this->_callback->rdp_input_mouse(flag, e->x(), e->y(), &(this->_keymap));
        //this->refresh(); 
    }
    
    void keyPressEvent(QKeyEvent *e) { 
        this->keyQtEvent(0x0000, e);
    }
    
    void keyReleaseEvent(QKeyEvent *e) {
        this->keyQtEvent(KBD_FLAG_UP, e);
    }
    
    void wheelEvent(QWheelEvent *e) {
        std::cout << "wheel " << " delta=" << e->delta() << std::endl;
    }
    
    bool eventFilter(QObject *obj, QEvent *e)
    {
        if (e->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            this->_callback->rdp_input_mouse(MOUSE_FLAG_MOVE, mouseEvent->x(), mouseEvent->y(), &(this->_keymap));
        }
        return false;
    }
    
    

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
    //-----------------------------------------
    //    SOCKET EVENTS LISTENING FUNCTIONS
    //-----------------------------------------
    
public Q_SLOTS:
    void readSck_And_ShowView() {
        if (this->_callback != nullptr) {
            this->reInitView();
            this->_callback->draw_event(time(nullptr));
            this->flush();
        }
    }
    
    
public:
    void setCallback_And_StartListening(mod_api* callback) {
        this->_callback = callback;
        QObject::connect(&(this->_sckRead), SIGNAL(activated(int)), this, SLOT(readSck_And_ShowView()));
    }
};


#endif
