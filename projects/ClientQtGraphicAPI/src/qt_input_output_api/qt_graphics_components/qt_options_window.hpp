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
   Author(s): Clément Moroldo

*/

#pragma once

#include "utils/log.hpp"

#include <vector>


#include "core/RDP/MonitorLayoutPDU.hpp"
#include "core/channel_list.hpp"
#include "client_redemption/client_input_output_api.hpp"



#include "../keymaps/qt_scancode_keymap.hpp"


#if REDEMPTION_QT_VERSION == 4
#   define REDEMPTION_QT_INCLUDE_WIDGET(name) <QtGui/name>
#else
#   define REDEMPTION_QT_INCLUDE_WIDGET(name) <QtWidgets/name>
#endif

#include REDEMPTION_QT_INCLUDE_WIDGET(QApplication)
#include REDEMPTION_QT_INCLUDE_WIDGET(QCheckBox)
#include REDEMPTION_QT_INCLUDE_WIDGET(QComboBox)
#include REDEMPTION_QT_INCLUDE_WIDGET(QDesktopWidget)
#include REDEMPTION_QT_INCLUDE_WIDGET(QDialog)
#include REDEMPTION_QT_INCLUDE_WIDGET(QFileDialog)
#include REDEMPTION_QT_INCLUDE_WIDGET(QFormLayout)
#include REDEMPTION_QT_INCLUDE_WIDGET(QGridLayout)
#include REDEMPTION_QT_INCLUDE_WIDGET(QLabel)
#include REDEMPTION_QT_INCLUDE_WIDGET(QLineEdit)
#include REDEMPTION_QT_INCLUDE_WIDGET(QPushButton)
#include REDEMPTION_QT_INCLUDE_WIDGET(QTabWidget)
#include REDEMPTION_QT_INCLUDE_WIDGET(QTableWidget)
#include REDEMPTION_QT_INCLUDE_WIDGET(QToolTip)
#include REDEMPTION_QT_INCLUDE_WIDGET(QWidget)

#undef REDEMPTION_QT_INCLUDE_WIDGET







class QtOptions : public QWidget
{

Q_OBJECT

public:
    enum : uint8_t {
        RDP,
        VNC
    };
    uint8_t protocol_type;
    ClientRedemptionIOAPI   * _front;
    const int            _width;
    const int            _height;



    QTabWidget         * _tabs;
    QGridLayout        * _layout;



    QWidget            * _connectionTab;
    QFormLayout        * _layoutConnection;

    QComboBox            profilComboBox;
    QLineEdit            profilLineEdit;
    QPushButton          _buttonRestorConfig;
    QPushButton          _buttonDelConfProfil;
    QCheckBox            _recordingCB;
    QCheckBox            _tlsBox;
    QCheckBox            _nlaBox;
    QComboBox            _languageComboBox;


    QLabel               _labelProfil;
    QLabel               _labelRecording;
    QLabel               _labelTls;
    QLabel               _labelNla;
    QLabel               _labelLanguage;



    QWidget            * _viewTab;
    QFormLayout        * _layoutView;

    QComboBox   _bppComboBox;
    QComboBox   _resolutionComboBox;
    QCheckBox   _spanCheckBox;
    QCheckBox   _wallpapperCheckBox;
    QCheckBox   windowdragCheckBox;
    QCheckBox   menuanimationsCheckBox;
    QCheckBox   cursorShadowCheckBox;
    QCheckBox   cursorsettingsCheckBox;
    QCheckBox   fontSmoothingCheckBox;
    QCheckBox   desktopCompositionCheckBox;

    QLabel               _labelBpp;
    QLabel               _labelResolution;
    QLabel               _labelSpan;
    QLabel               _labelWallpaper;
    QLabel   windowdragLabel;
    QLabel   menuanimationsLabel;
    QLabel   cursorShadowLabel;
    QLabel   cursorsettingsLabel;
    QLabel   fontSmoothingLabel;
    QLabel   desktopCompositionLabel;



    QWidget            * _servicesTab;
    QFormLayout        * _layoutServices;

    QCheckBox            _clipboardCheckBox;
    QCheckBox            _shareCheckBox;
    QLineEdit            _sharePath;
    QPushButton          _buttonSharePath;
    QCheckBox            _soundBox;
    QCheckBox            remoteappCheckBox;
    QLineEdit            remoteapp_cmd;
    QLineEdit            remoteapp_workin_dir;

    QLabel               _labelClipboard;
    QLabel               _labelShare;
    QLabel               _labelSharePath;
    QLabel               _labelSound;
    QLabel               remoteappLabel;
    QLabel               remoteapp_cmd_label;
    QLabel               remoteapp_workin_dir_label;

//     QWidget            * _keyboardTab;
//     QPushButton        * _buttonDeleteKey;
//     QPushButton        * _buttonAddKey;
//     QFormLayout        * _layoutKeyboard;
//     QTableWidget       * _tableKeySetting;
//     const int            _columnNumber;
//     const int            _tableKeySettingMaxHeight;


    QtOptions(ClientRedemptionIOAPI * front, uint8_t protocol_type, QWidget * parent)
        : QWidget(parent)
        , protocol_type(protocol_type)
        , _front(front)
        , _width(400)
        , _height(330)


        , _tabs(nullptr)
        , _layout(nullptr)


        , _connectionTab(nullptr)
        , _layoutConnection(nullptr)

        , profilComboBox(this)
        , profilLineEdit("", this)
        , _buttonRestorConfig("Default configuration", this)
        , _buttonDelConfProfil("Delete current Profile", this)
        , _recordingCB(this)
        , _tlsBox(this)
        , _nlaBox(this)
        , _languageComboBox(this)

        , _labelProfil("Options Profil:", this)
        , _labelRecording("Record movie :", this)
        , _labelTls("TLS :", this)
        , _labelNla("NLA :", this)
        , _labelLanguage("Keyboard Language :", this)


        , _viewTab(nullptr)
        , _layoutView(nullptr)

        , _bppComboBox(this)
        , _resolutionComboBox(this)
        , _spanCheckBox(this)
        , _wallpapperCheckBox(this)
        , windowdragCheckBox(this)
        , menuanimationsCheckBox(this)
        , cursorShadowCheckBox(this)
        , cursorsettingsCheckBox(this)
        , fontSmoothingCheckBox(this)
        , desktopCompositionCheckBox( this)

        , _labelBpp("Color depth :", this)
        , _labelResolution("Resolution :", this)
        , _labelSpan("Span screen :", this)
        , _labelWallpaper("Enable wallaper :", this)
        , windowdragLabel("Enable windowdrag :", this)
        , menuanimationsLabel("Enable menu animations :", this)
        , cursorShadowLabel("Enable cursor shadow", this)
        , cursorsettingsLabel("Enable cursor settings :", this)
        , fontSmoothingLabel("Enable font smoothing :", this)
        , desktopCompositionLabel("Enable desktop composition :", this)


        , _servicesTab(nullptr)
        , _layoutServices(nullptr)

        , _clipboardCheckBox(this)
        , _shareCheckBox(this)
        , _sharePath(this->_front->SHARE_DIR.c_str(), this)
        , _buttonSharePath("Select a Directory", this)
        , _soundBox(this)
        , remoteappCheckBox(this)
        , remoteapp_cmd(this->_front->full_cmd_line.c_str(), this)
        , remoteapp_workin_dir(this->_front->source_of_WorkingDir.c_str(), this)

        , _labelClipboard("Shared Clipboard :", this)
        , _labelShare("Shared Virtual Disk :", this)
        , _labelSharePath("Shared Path :", this)
        , _labelSound("Sound :",  this)
        , remoteappLabel("Enable remote app :", this)
        , remoteapp_cmd_label("Command line :", this)
        , remoteapp_workin_dir_label("Working direction :", this)

//         , _keyboardTab(nullptr)
//         , _buttonDeleteKey(nullptr)
//         , _buttonAddKey(nullptr)

//         , _layoutKeyboard(nullptr)
//         , _tableKeySetting(nullptr)
//         , _columnNumber(4)
//         , _tableKeySettingMaxHeight((20*6)+11)
    {
        this->setFixedSize(this->_width, this->_height);
        this->_front->setClientInfo();
        this->_layout = new QGridLayout(this);



        // Tab options
        this->_viewTab = new QWidget(this);
        this->_connectionTab = new QWidget(this);
        this->_servicesTab = new QWidget(this);
// /*        this->_keyboardTab = new QWidget(this)*/;
        this->_tabs = new QTabWidget(this);



        // Connection config
        const QString strConnection(" General ");
        this->_layoutConnection = new QFormLayout(this->_connectionTab);

        this->profilComboBox.setLineEdit(&(this->profilLineEdit));
        for (size_t i = 0; i < this->_front->userProfils.size(); i++) {
            this->profilComboBox.addItem(this->_front->userProfils[i].name.c_str(), this->_front->userProfils[i].id);
        }
        this->profilComboBox.setStyleSheet("combobox-popup: 0;");
        this->_layoutConnection->addRow(&(this->_labelProfil), &(this->profilComboBox));
        this->QObject::connect(&(this->profilComboBox), SIGNAL(currentIndexChanged(int)), this, SLOT(changeProfil(int)));

        this->_buttonRestorConfig.setFixedSize(160, 20);
        this->_buttonRestorConfig.setCursor(Qt::PointingHandCursor);
        this->QObject::connect(&(this->_buttonRestorConfig) , SIGNAL (pressed()) , this, SLOT (restoreConfig()));

        this->_buttonDelConfProfil.setFixedSize(160, 20);
        this->_buttonDelConfProfil.setCursor(Qt::PointingHandCursor);
        this->QObject::connect(&(this->_buttonDelConfProfil) , SIGNAL (pressed()) , this, SLOT (deleteCurrentProtile()));
        this->_layoutConnection->addRow(&(this->_buttonRestorConfig), &(this->_buttonDelConfProfil));

        this->_layoutConnection->addRow(&(this->_labelRecording), &(this->_recordingCB));
        this->QObject::connect(&(this->_recordingCB), SIGNAL(stateChanged(int)), this, SLOT(recordingCheckChange(int)));

        this->_tlsBox.setCheckState(Qt::Unchecked);
        this->_layoutConnection->addRow(&(this->_labelTls), &(this->_tlsBox));

        this->_nlaBox.setCheckState(Qt::Unchecked);
        this->_layoutConnection->addRow(&(this->_labelNla), &(this->_nlaBox));

        for (int i = 0; i < KEYLAYOUTS_LIST_SIZE; i++) {
            this->_languageComboBox.addItem(keylayoutsList[i]->locale_name, keylayoutsList[i]->LCID);
        }
        this->_languageComboBox.setStyleSheet("combobox-popup: 0;");
        this->_layoutConnection->addRow(&(this->_labelLanguage), &(this->_languageComboBox));

        this->_connectionTab->setLayout(this->_layoutConnection);
        this->_tabs->addTab(this->_connectionTab, strConnection);



        // VIEW TAB
        const QString strView("View");
        this->_layoutView = new QFormLayout(this->_viewTab);

        this->_bppComboBox.addItem("15", 15);
        this->_bppComboBox.addItem("16", 16);
        this->_bppComboBox.addItem("24", 24);
        this->_bppComboBox.setStyleSheet("combobox-popup: 0;");
        this->_layoutView->addRow(&(this->_labelBpp), &(this->_bppComboBox));

        this->_resolutionComboBox.addItem( "640 * 480", 640);
        this->_resolutionComboBox.addItem( "800 * 600", 800);
        this->_resolutionComboBox.addItem("1024 * 768", 1024);
        this->_resolutionComboBox.addItem("1600 * 900", 1600);
        this->_resolutionComboBox.setStyleSheet("combobox-popup: 0;");
        this->_layoutView->addRow(&(this->_labelResolution), &(this->_resolutionComboBox));

        this->_spanCheckBox.setCheckState(Qt::Unchecked);
        this->_layoutView->addRow(&(this->_labelSpan), &(this->_spanCheckBox));
        this->QObject::connect(&(this->_spanCheckBox), SIGNAL(stateChanged(int)), this, SLOT(spanCheckChange(int)));

        this->_layoutView->addRow(&(this->_labelWallpaper), &(this->_wallpapperCheckBox));
        this->_layoutView->addRow(&(this->windowdragLabel), &(this->windowdragCheckBox));
        this->_layoutView->addRow(&(this->menuanimationsLabel), &(this->menuanimationsCheckBox));
        this->_layoutView->addRow(&(this->cursorShadowLabel), &(this->cursorShadowCheckBox));
        this->_layoutView->addRow(&(this->cursorsettingsLabel), &(this->cursorsettingsCheckBox));
        this->_layoutView->addRow(&(this->fontSmoothingLabel), &(this->fontSmoothingCheckBox));
        this->_layoutView->addRow(&(this->desktopCompositionLabel), &(this->desktopCompositionCheckBox));

        this->_viewTab->setLayout(this->_layoutView);
        this->_tabs->addTab(this->_viewTab, strView);



        // Services tab
        const QString strServices("Services");
        this->_layoutServices = new QFormLayout(this->_servicesTab);

        this->_clipboardCheckBox.setCheckState(Qt::Unchecked);
        this->_layoutServices->addRow(&(this->_labelClipboard), &(this->_clipboardCheckBox));

        this->_shareCheckBox.setCheckState(Qt::Unchecked);
        this->QObject::connect(&(this->_shareCheckBox), SIGNAL(stateChanged(int)), this, SLOT(setEnableSharePath(int)));
        this->_layoutServices->addRow(&(this->_labelShare), &(this->_shareCheckBox));

        this->_sharePath.setEnabled(this->_front->enable_shared_virtual_disk);
        this->_layoutServices->addRow(&(this->_labelSharePath), &(this->_sharePath));

        QRect rectPath(QPoint(190, 226),QSize(180, 24));
        this->_buttonSharePath.setGeometry(rectPath);
        this->_buttonSharePath.setCursor(Qt::PointingHandCursor);
        this->QObject::connect(&(this->_buttonSharePath) , SIGNAL (pressed()) , this, SLOT (dirPathPressed()));
        QLabel dirButtonLabel("", this);
        this->_layoutServices->addRow(&(dirButtonLabel), &(this->_buttonSharePath));

        this->_soundBox.setCheckState(Qt::Unchecked);
        this->_layoutServices->addRow(&(this->_labelSound), &(this->_soundBox));

        this->remoteappCheckBox.setCheckState(Qt::Unchecked);
        if (this->_front->mod_state == ClientRedemptionIOAPI::MOD_RDP_REMOTE_APP) {
            this->remoteappCheckBox.setCheckState(Qt::Checked);
        }
        this->QObject::connect(&(this->remoteappCheckBox), SIGNAL(stateChanged(int)), this, SLOT(setEnableRemoteApp(int)));
        this->_layoutServices->addRow(&(this->remoteappLabel), &(this->remoteappCheckBox));

        this->remoteapp_cmd.setEnabled(this->_front->mod_state == ClientRedemptionIOAPI::MOD_RDP_REMOTE_APP);
        this->_layoutServices->addRow(&(this->remoteapp_cmd_label), &(this->remoteapp_cmd));

        this->remoteapp_workin_dir.setEnabled(this->_front->mod_state == ClientRedemptionIOAPI::MOD_RDP_REMOTE_APP);
        this->_layoutServices->addRow(&(this->remoteapp_workin_dir_label), &(this->remoteapp_workin_dir));

        this->_servicesTab->setLayout(this->_layoutServices);
        this->_tabs->addTab(this->_servicesTab, strServices);




        // Keyboard tab
//         const QString strKeyboard("Keyboard");
//         this->_layoutKeyboard = new QFormLayout(this->_keyboardTab);

//         for (int i = 0; i < KEYLAYOUTS_LIST_SIZE; i++) {
//             this->_languageComboBox.addItem(keylayoutsList[i]->locale_name, keylayoutsList[i]->LCID);
//         }
//         this->_languageComboBox.setStyleSheet("combobox-popup: 0;");
//         this->_layoutKeyboard->addRow(new QLabel("", this));
//         this->_layoutKeyboard->addRow(&(this->_labelLanguage), &(this->_languageComboBox));

//         this->_tableKeySetting = new QTableWidget(0, this->_columnNumber, this);
//         QList<QString> columnTitles;
//         columnTitles << "Qt key ID" << "Scan Code" << "ASCII8" << "Extended";
//         this->_tableKeySetting->setHorizontalHeaderLabels({columnTitles});
//         this->_tableKeySetting->setColumnWidth(0 ,85);
//         this->_tableKeySetting->setColumnWidth(1 ,84);
//         this->_tableKeySetting->setColumnWidth(2 ,84);
//         this->_tableKeySetting->setColumnWidth(3 ,74);


//         for (size_t i = 0; i < this->_front->keyCustomDefinitions.size(); i++) {
//             this->addRow();
//             this->setRowValues(this->_front->keyCustomDefinitions[i].qtKeyID,
//                                this->_front->keyCustomDefinitions[i].scanCode,
//                                this->_front->keyCustomDefinitions[i].ASCII8,
//                                this->_front->keyCustomDefinitions[i].extended);
//
//         }


//         std::ifstream ifichier(this->_front->MAIN_DIR + std::string(KEY_SETTING_PATH), std::ios::in);
//         if(ifichier) {
//
//             std::string ligne;
//             std::string delimiter = " ";
//
//             while(getline(ifichier, ligne)) {
//
//                 int pos(ligne.find(delimiter));
//
//                 if (strcmp(ligne.substr(0, pos).c_str(), "-") == 0) {
//
//                     ligne = ligne.substr(pos + delimiter.length(), ligne.length());
//                     pos = ligne.find(delimiter);
//
//                     int qtKeyID  = std::stoi(ligne.substr(0, pos));
//                     ligne = ligne.substr(pos + delimiter.length(), ligne.length());
//                     pos = ligne.find(delimiter);
//
//                     int scanCode = std::stoi(ligne.substr(0, pos));
//                     ligne = ligne.substr(pos + delimiter.length(), ligne.length());
//                     pos = ligne.find(delimiter);
//
//                     int ASCII8   = std::stoi(ligne.substr(0, pos));
//                     ligne = ligne.substr(pos + delimiter.length(), ligne.length());
//                     pos = ligne.find(delimiter);
//
//                     int extended = std::stoi(ligne.substr(0, pos));
//
//                     this->_front->qtRDPKeymap.setCustomKeyCode(qtKeyID, scanCode, ASCII8, extended);
//
//                     this->addRow();
//                     this->setRowValues(qtKeyID, scanCode, ASCII8, extended);
//                 }
//             }
//
//             ifichier.close();
//         }
//          this->addRow();
//
//
//
//         this->_layoutKeyboard->addRow(this->_tableKeySetting);
//
//         this->_keyboardTab->setLayout(this->_layoutKeyboard);
//
//
//         this->_tabs->addTab(this->_keyboardTab, strKeyboard);
//
//         this->_buttonAddKey = new QPushButton("Add row", this->_keyboardTab);
//         QRect rectAddKey(QPoint(110, 226),QSize(70, 24));
//         this->_buttonAddKey->setToolTip(this->_buttonAddKey->text());
//         this->_buttonAddKey->setGeometry(rectAddKey);
//         this->_buttonAddKey->setCursor(Qt::PointingHandCursor);
//         this->QObject::connect(this->_buttonAddKey    , SIGNAL (pressed()) , this, SLOT (addRow()));
//
//
//         this->_buttonDeleteKey = new QPushButton("Delete selected row", this->_keyboardTab);
//         QRect rectDeleteKey(QPoint(190, 226),QSize(180, 24));
//         this->_buttonDeleteKey->setToolTip(this->_buttonDeleteKey->text());
//         this->_buttonDeleteKey->setGeometry(rectDeleteKey);
//         this->_buttonDeleteKey->setCursor(Qt::PointingHandCursor);
//         this->QObject::connect(this->_buttonDeleteKey , SIGNAL (pressed()) , this, SLOT (deletePressed()));

        this->_layout->addWidget(this->_tabs, 0, 0, 9, 4);

        this->setLayout(this->_layout);


        this->setConfigValues();

    }


private:
    void setConfigValues() {
        int indexProfil = this->profilComboBox.findData(this->_front->current_user_profil);
        if ( indexProfil != -1 ) {
            this->profilComboBox.setCurrentIndex(indexProfil);
        }

        int indexBpp = this->_bppComboBox.findData(this->_front->info.bpp);
        if ( indexBpp != -1 ) {
            this->_bppComboBox.setCurrentIndex(indexBpp);
        }

        int indexResolution = this->_resolutionComboBox.findData(this->_front->rdp_width);
        if ( indexResolution != -1 ) {
            this->_resolutionComboBox.setCurrentIndex(indexResolution);
        }

        if (this->_front->is_spanning) {
            this->_spanCheckBox.setCheckState(Qt::Checked);
        } else {
            this->_spanCheckBox.setCheckState(Qt::Unchecked);
        }

        if (this->_front->info.rdp5_performanceflags == PERF_DISABLE_WALLPAPER) {
            this->_wallpapperCheckBox.setCheckState(Qt::Checked);
        }

        int indexLanguage = this->_languageComboBox.findData(this->_front->info.keylayout);
        if ( indexLanguage != -1 ) {
            this->_languageComboBox.setCurrentIndex(indexLanguage);
        }

        if (this->_front->is_recording) {
            this->_recordingCB.setCheckState(Qt::Checked);
        } else {
            this->_recordingCB.setCheckState(Qt::Unchecked);
        }

        if (this->_front->modRDPParamsData.enable_tls) {
            this->_tlsBox.setCheckState(Qt::Checked);
        } else {
            this->_tlsBox.setCheckState(Qt::Unchecked);
        }

        if (this->_front->modRDPParamsData.enable_nla) {
            this->_nlaBox.setCheckState(Qt::Checked);
        } else {
            this->_nlaBox.setCheckState(Qt::Unchecked);
        }

        if (this->_front->modRDPParamsData.enable_sound) {
            this->_soundBox.setCheckState(Qt::Checked);
        } else {
            this->_soundBox.setCheckState(Qt::Unchecked);
        }

        if (this->_front->enable_shared_clipboard) {
            this->_clipboardCheckBox.setCheckState(Qt::Checked);
        } else {
            this->_clipboardCheckBox.setCheckState(Qt::Unchecked);
        }

        if (this->_front->enable_shared_virtual_disk) {
            this->_shareCheckBox.setCheckState(Qt::Checked);
        } else {
            this->_shareCheckBox.setCheckState(Qt::Unchecked);
        }
    }

//     void setRowValues(int qtKeyID, int scanCode, int ASCII8, int extended) {
//         int row(this->_tableKeySetting->rowCount() - 1);
//
//         QTableWidgetItem * item1 = new QTableWidgetItem;
//         item1->setText(std::to_string(qtKeyID).c_str());
//         this->_tableKeySetting->setItem(row, 0, item1);
//
//         QTableWidgetItem * item2 = new QTableWidgetItem;
//         item2->setText(std::to_string(scanCode).c_str());
//         this->_tableKeySetting->setItem(row, 1, item2);
//
//         QTableWidgetItem * item3 = new QTableWidgetItem;
//         item3->setText(std::to_string(ASCII8).c_str());
//         this->_tableKeySetting->setItem(row, 2, item3);
//
//         static_cast<QComboBox*>(this->_tableKeySetting->cellWidget(row, 3))->setCurrentIndex(extended);
//     }
//
//     void updateKeySetting() {
//         int tableKeySettingHeight((20*(this->_tableKeySetting->rowCount()+1))+11);
//         if (tableKeySettingHeight > this->_tableKeySettingMaxHeight) {
//             tableKeySettingHeight = this->_tableKeySettingMaxHeight;
//         }
//         this->_tableKeySetting->setFixedSize((80*this->_columnNumber)+40, tableKeySettingHeight);
//         if (this->_tableKeySetting->rowCount() > 5) {
//             this->_tableKeySetting->setColumnWidth(3 ,74);
//         } else {
//             this->_tableKeySetting->setColumnWidth(3 ,87);
//         }
//
//         this->update();
//     }

 void setConfigValues() {

        bool new_profil = true;
        std::string text_profil = this->profilComboBox.currentText().toStdString();
        for (size_t i = 0; i < this->_front->userProfils.size(); i++) {
            if (this->_front->userProfils[i].name == text_profil) {
                new_profil = false;
            }
        }

        if (new_profil) {
            this->_front->userProfils.push_back({int(this->_front->userProfils.size()), text_profil.c_str()});
            this->_front->current_user_profil = this->_front->userProfils.size()-1;
        } else {
             this->_front->current_user_profil = this->profilComboBox.currentIndex();
        }

        this->_front->info.bpp = this->_bppComboBox.currentText().toInt();
        std::string delimiter = " * ";
        std::string resolution( this->_resolutionComboBox.currentText().toStdString());
        int pos(resolution.find(delimiter));
        this->_front->rdp_width  = std::stoi(resolution.substr(0, pos));
        this->_front->rdp_height = std::stoi(resolution.substr(pos + delimiter.length(), resolution.length()));
        if (this->_wallpapperCheckBox.isChecked()) {
            this->_front->info.rdp5_performanceflags = PERF_DISABLE_WALLPAPER;
        } else {
            this->_front->info.rdp5_performanceflags = 0;
        }
        if (this->_spanCheckBox.isChecked()) {
            this->_front->is_spanning = true;
        } else {
            this->_front->is_spanning = false;
        }
        if (this->_recordingCB.isChecked()) {
            this->_front->is_recording = true;
        } else {
            this->_front->is_recording = false;
        }
        if (this->_tlsBox.isChecked()) {
            this->_front->modRDPParamsData.enable_tls = true;
        } else {
            this->_front->modRDPParamsData.enable_tls = false;
        }
        if (this->_nlaBox.isChecked()) {
            this->_front->modRDPParamsData.enable_nla = true;
        } else {
            this->_front->modRDPParamsData.enable_nla = false;
        }
        if (this->_soundBox.isChecked()) {
            this->_front->modRDPParamsData.enable_sound = true;
        } else {
            this->_front->modRDPParamsData.enable_sound = false;
        }
        this->_front->info.keylayout = this->_languageComboBox.itemData(this->_languageComboBox.currentIndex()).toInt();
        this->_front->update_keylayout();

        if (this->_clipboardCheckBox.isChecked()) {
            this->_front->enable_shared_clipboard = true;
        } else {
            this->_front->enable_shared_clipboard = false;
        }

        if (this->_shareCheckBox.isChecked()) {
            this->_front->enable_shared_virtual_disk = true;
        } else {
            this->_front->enable_shared_virtual_disk = false;
        }

        this->_front->SHARE_DIR = this->_sharePath.text().toStdString();

        this->_front->writeClientInfo();
    }


public Q_SLOTS:
    void deleteCurrentProtile() {
        if (this->profilComboBox.currentIndex() != 0) {
            this->_front->deleteCurrentProtile();
            this->profilComboBox.removeItem(this->_front->current_user_profil);
            this->changeProfil(0);
        }
    }

    void restoreConfig() {
        this->_front->setDefaultConfig();
        this->setConfigValues();
    }

    void changeProfil(int index) {
        this->_front->current_user_profil = this->profilComboBox.itemData(index).toInt();
        this->_front->setClientInfo();
        this->setConfigValues();
    }

    void setEnableSharePath(int value) {
        this->_sharePath.setEnabled(value);
        this->_buttonSharePath.setEnabled(value);
        this->_front->enable_shared_virtual_disk = bool(value);
    }

    void setEnableRemoteApp(int value) {
        this->remoteapp_cmd.setEnabled(value);
        this->remoteapp_workin_dir.setEnabled(value);
        if (value) {
            this->_front->mod_state = ClientRedemptionIOAPI::MOD_RDP_REMOTE_APP;
        } else {
            this->_front->mod_state = ClientRedemptionIOAPI::MOD_RDP;
        }
    }


    void dirPathPressed() {
        QString filePath("");
        filePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     this->_front->SHARE_DIR.c_str(),
                                                     QFileDialog::ShowDirsOnly |
                                                     QFileDialog::DontResolveSymlinks);
        std::string str_share_path(filePath.toStdString());
        this->_sharePath.setText(filePath);
    }

    void savePressed() {}

    void saveReleased() {

        bool new_profil = true;
        std::string text_profil = this->profilComboBox.currentText().toStdString();
        for (size_t i = 0; i < this->_front->userProfils.size(); i++) {
            if (this->_front->userProfils[i].name == text_profil) {
                new_profil = false;
            }
        }

        if (new_profil) {
            this->_front->userProfils.push_back({int(this->_front->userProfils.size()), text_profil.c_str()});
            this->_front->current_user_profil = this->_front->userProfils.size()-1;
        } else {
             this->_front->current_user_profil = this->profilComboBox.currentIndex();
        }

        this->_front->info.bpp = this->_bppComboBox.currentText().toInt();
        std::string delimiter = " * ";
        std::string resolution( this->_resolutionComboBox.currentText().toStdString());
        int pos(resolution.find(delimiter));
        this->_front->rdp_width  = std::stoi(resolution.substr(0, pos));
        this->_front->rdp_height = std::stoi(resolution.substr(pos + delimiter.length(), resolution.length()));
        if (this->_wallpapperCheckBox.isChecked()) {
            this->_front->info.rdp5_performanceflags = PERF_DISABLE_WALLPAPER;
        } else {
            this->_front->info.rdp5_performanceflags = 0;
        }
        if (this->_spanCheckBox.isChecked()) {
            this->_front->is_spanning = true;
        } else {
            this->_front->is_spanning = false;
        }
        if (this->_recordingCB.isChecked()) {
            this->_front->is_recording = true;
        } else {
            this->_front->is_recording = false;
        }
        if (this->_tlsBox.isChecked()) {
            this->_front->modRDPParamsData.enable_tls = true;
        } else {
            this->_front->modRDPParamsData.enable_tls = false;
        }
        if (this->_nlaBox.isChecked()) {
            this->_front->modRDPParamsData.enable_nla = true;
        } else {
            this->_front->modRDPParamsData.enable_nla = false;
        }
        if (this->_soundBox.isChecked()) {
            this->_front->modRDPParamsData.enable_sound = true;
        } else {
            this->_front->modRDPParamsData.enable_sound = false;
        }
        this->_front->info.keylayout = this->_languageComboBox.itemData(this->_languageComboBox.currentIndex()).toInt();
        this->_front->update_keylayout();

        if (this->_clipboardCheckBox.isChecked()) {
            this->_front->enable_shared_clipboard = true;
        } else {
            this->_front->enable_shared_clipboard = false;
        }

        if (this->_shareCheckBox.isChecked()) {
            this->_front->enable_shared_virtual_disk = true;
        } else {
            this->_front->enable_shared_virtual_disk = false;
        }

        this->_front->SHARE_DIR = this->_sharePath.text().toStdString();

        this->_front->writeClientInfo();

//         remove((this->_front->MAIN_DIR + std::string(KEY_SETTING_PATH)).c_str());
//         this->_front->qtRDPKeymap.clearCustomKeyCode();

//         std::ofstream ofichier(this->_front->MAIN_DIR + std::string(KEY_SETTING_PATH), std::ios::out | std::ios::trunc);
//         if(ofichier) {
//
//             ofichier << "Key Setting" << std::endl << std::endl;
//
//             const int row_count = this->_tableKeySetting->rowCount();
//
//             for (int i = 0; i < row_count; i++) {
//                 int qtKeyID(0);
//                 if (!(this->_tableKeySetting->item(i, 0)->text().isEmpty())) {
//                     qtKeyID = this->_tableKeySetting->item(i, 0)->text().toInt();
//                 }
//
//                 if (qtKeyID != 0) {
//                     int scanCode(0);
//                     if (!(this->_tableKeySetting->item(i, 0)->text().isEmpty())) {
//                         scanCode = this->_tableKeySetting->item(i, 1)->text().toInt();
//                     }
//                     int ASCII8(0);
//                     if (!(this->_tableKeySetting->item(i, 0)->text().isEmpty())) {
//                         ASCII8 = this->_tableKeySetting->item(i, 2)->text().toInt();
//                     }
//                     int extended(static_cast<QComboBox*>(this->_tableKeySetting->cellWidget(i, 3))->currentIndex());
//
//                     this->_front->qtRDPKeymap.setCustomKeyCode(qtKeyID, scanCode, ASCII8, extended);
//
//                     ofichier << "- ";
//                     ofichier << qtKeyID  << " ";
//                     ofichier << scanCode << " ";
//                     ofichier << ASCII8   << " ";
//                     ofichier << extended << std::endl;
//                 }
//             }
//             ofichier.close();
//         }

        this->close();
    }

    void cancelPressed() {}

    void cancelReleased() {
        this->close();
    }

//     void addRow() {
//         int rowNumber(this->_tableKeySetting->rowCount());
//         this->_tableKeySetting->insertRow(rowNumber);
//         this->_tableKeySetting->setRowHeight(rowNumber ,20);
//         QComboBox * combo = new QComboBox(this->_tableKeySetting);
//         combo->addItem("No" , 0);
//         combo->addItem("Yes", 1);
//         this->_tableKeySetting->setCellWidget(rowNumber, 3, combo);
//
//         QTableWidgetItem * item1 = new QTableWidgetItem;
//         item1->setText("");
//         this->_tableKeySetting->setItem(rowNumber, 0, item1);
//
//         QTableWidgetItem * item2 = new QTableWidgetItem;
//         item2->setText("");
//         this->_tableKeySetting->setItem(rowNumber, 1, item2);
//
//         QTableWidgetItem * item3 = new QTableWidgetItem;
//         item3->setText("");
//         this->_tableKeySetting->setItem(rowNumber, 2, item3);
//
//
//         this->updateKeySetting();
//     }

//     void deletePressed() {
//        QModelIndexList indexes = this->_tableKeySetting->selectionModel()->selection().indexes();
//        for (int i = 0; i < indexes.count(); ++i) {
//            QModelIndex index = indexes.at(i);
//            this->_tableKeySetting->removeRow(index.row());
//        }
//
//        if (this->_tableKeySetting->rowCount() == 0) {
//            this->addRow();
//        }
//
//        this->updateKeySetting();
//     }

    void deleteReleased() {}

    void spanCheckChange(int state) {
        if (state == Qt::Unchecked) {
//             this->_monitorCountComboBox.setCurrentIndex(0);
        }
    }

    void recordingCheckChange(int ) {
//         if (state == Qt::Unchecked) {
//             this->_captureSnapFreqComboBox.setEnabled(false);
//         } else {
//             this->_captureSnapFreqComboBox.setEnabled(true);
//         }
    }

    void monitorCountkChange(int index) {
        if (index != 0) {
            this->_spanCheckBox.setCheckState(Qt::Checked);
        }
    }

};




