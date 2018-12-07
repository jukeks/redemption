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

#include "client_redemption/client_config/client_redemption_config.hpp"

ClientRedemptionConfig::ClientRedemptionConfig(SessionReactor& session_reactor, char const* argv[], int argc, RDPVerbose verbose, FrontAPI &front, const std::string &MAIN_DIR )
: MAIN_DIR((MAIN_DIR.empty() || MAIN_DIR == "/")
    ? MAIN_DIR
    : (MAIN_DIR.back() == '/')
    ? MAIN_DIR.substr(0, MAIN_DIR.size() - 1)
    : MAIN_DIR)
, verbose(verbose)
//, _recv_disconnect_ultimatum(false)
, wab_diag_question(false)
, quick_connection_test(true)
, persist(false)
, time_out_disconnection(5000)
, keep_alive_freq(100)
, windowsData(this->WINDOWS_CONF)
, vnc_conf(session_reactor, front)
{
    this->setDefaultConfig();

    this->info.screen_info.width       = 800;
    this->info.screen_info.height      = 600;
    this->info.keylayout               = 0x040C;            // 0x40C FR, 0x409 USA
    this->info.console_session         = false;
    this->info.brush_cache_code        = 0;
    this->info.screen_info.bpp         = BitsPerPixel{24};
    this->info.rdp5_performanceflags   = PERF_DISABLE_WALLPAPER;
    this->info.cs_monitor.monitorCount = 1;

    this->rDPRemoteAppConfig.source_of_ExeOrFile  = "C:\\Windows\\system32\\notepad.exe";
    this->rDPRemoteAppConfig.source_of_WorkingDir = "C:\\Users\\user1";

    this->rDPRemoteAppConfig.full_cmd_line = this->rDPRemoteAppConfig.source_of_ExeOrFile + " " + this->rDPRemoteAppConfig.source_of_Arguments;

    if (!this->MAIN_DIR.empty()) {
        for (auto* pstr : {
            &this->DATA_DIR,
            &this->REPLAY_DIR,
            &this->CB_TEMP_DIR,
            &this->DATA_CONF_DIR,
            &this->SOUND_TEMP_DIR
        }) {
            if (!pstr->empty()) {
                if (!file_exist(pstr->c_str())) {
                    LOG(LOG_INFO, "Create file \"%s\".", pstr->c_str());
                    mkdir(pstr->c_str(), 0775);
                }
            }
        }
    }

    // Set RDP CLIPRDR config
    this->rDPClipboardConfig.arbitrary_scale = 40;
    this->rDPClipboardConfig.server_use_long_format_names = true;
    this->rDPClipboardConfig.cCapabilitiesSets = 1;
    this->rDPClipboardConfig.generalFlags = RDPECLIP::CB_STREAM_FILECLIP_ENABLED | RDPECLIP::CB_FILECLIP_NO_FILE_PATHS;
    this->rDPClipboardConfig.add_format(ClientCLIPRDRConfig::CF_QT_CLIENT_FILEGROUPDESCRIPTORW, RDPECLIP::FILEGROUPDESCRIPTORW.data());
    this->rDPClipboardConfig.add_format(ClientCLIPRDRConfig::CF_QT_CLIENT_FILECONTENTS, RDPECLIP::FILECONTENTS.data());
    this->rDPClipboardConfig.add_format(RDPECLIP::CF_TEXT, {});
    this->rDPClipboardConfig.add_format(RDPECLIP::CF_METAFILEPICT, {});
    this->rDPClipboardConfig.path = this->CB_TEMP_DIR;


    // Set RDP RDPDR config
    this->rDPDiskConfig.add_drive(this->SHARE_DIR, rdpdr::RDPDR_DTYP_FILESYSTEM);
    this->rDPDiskConfig.enable_drive_type = true;
    this->rDPDiskConfig.enable_printer_type = true;


    // Set RDP SND config
    this->rDPSoundConfig.dwFlags = rdpsnd::TSSNDCAPS_ALIVE | rdpsnd::TSSNDCAPS_VOLUME;
    this->rDPSoundConfig.dwVolume = 0x7fff7fff;
    this->rDPSoundConfig.dwPitch = 0;
    this->rDPSoundConfig.wDGramPort = 0;
    this->rDPSoundConfig.wNumberOfFormats = 1;
    this->rDPSoundConfig.wVersion = 0x06;



    this->userProfils.emplace_back(0, "Default");

    if (!this->MAIN_DIR.empty()) {
        this->setUserProfil();
        this->setClientInfo();
        this->setCustomKeyConfig();
        this->setAccountData();

        this->openWindowsData();
    }
    std::fill(std::begin(this->info.order_caps.orderSupport), std::end(this->info.order_caps.orderSupport), 1);
    this->info.glyph_cache_caps.GlyphSupportLevel = GlyphCacheCaps::GLYPH_SUPPORT_FULL;

//         this->parse_options(argc, argv);


    auto options = cli::options(
        cli::helper("Client ReDemPtion Help menu."),

        cli::option('h', "help").help("Show help")
        .action(cli::help),

        cli::option('v', "version").help("Show version")
        .action(cli::quit([]{ std::cout << redemption_info_version() << "\n"; })),

        cli::helper("========= Connection ========="),

        cli::option('u', "username").help("Set target session user name")
        .action(cli::arg([this](std::string s){
            this->user_name = std::move(s);

            this->connection_info_cmd_complete |= NAME_GOT;
        })),

        cli::option('p', "password").help("Set target session user password")
        .action(cli::arg([this](std::string s){
            this->user_password = std::move(s);

            this->connection_info_cmd_complete |= PWD_GOT;
        })),

        cli::option('i', "ip").help("Set target IP address")
        .action(cli::arg([this](std::string s){
            this->target_IP = std::move(s);

            this->connection_info_cmd_complete |= IP_GOT;
        })),

        cli::option('P', "port").help("Set port to use on target")
        .action(cli::arg([this](int n){
            this->port = n;
            this->connection_info_cmd_complete |= PORT_GOT;
        })),

        cli::helper("========= Verbose ========="),

        cli::option("rdpdr").help("Active rdpdr logs")
        .action(cli::on_off_bit_location<RDPVerbose::rdpdr>(this->verbose)),

        cli::option("rdpsnd").help("Active rdpsnd logs")
        .action(cli::on_off_bit_location<RDPVerbose::rdpsnd>(this->verbose)),

        cli::option("cliprdr").help("Active cliprdr logs")
        .action(cli::on_off_bit_location<RDPVerbose::cliprdr>(this->verbose)),

        cli::option("graphics").help("Active graphics logs")
        .action(cli::on_off_bit_location<RDPVerbose::graphics>(this->verbose)),

        cli::option("printer").help("Active printer logs")
        .action(cli::on_off_bit_location<RDPVerbose::printer>(this->verbose)),

        cli::option("rdpdr-dump").help("Actives rdpdr logs and dump brute rdpdr PDU")
        .action(cli::on_off_bit_location<RDPVerbose::rdpdr_dump>(this->verbose)),

        cli::option("cliprd-dump").help("Actives cliprdr logs and dump brute cliprdr PDU")
        .action(cli::on_off_bit_location<RDPVerbose::cliprdr_dump>(this->verbose)),

        cli::option("basic-trace").help("Active basic-trace logs")
        .action(cli::on_off_bit_location<RDPVerbose::basic_trace>(this->verbose)),

        cli::option("connection").help("Active connection logs")
        .action(cli::on_off_bit_location<RDPVerbose::connection>(this->verbose)),

        cli::option("rail-order").help("Active rail-order logs")
        .action(cli::on_off_bit_location<RDPVerbose::rail_order>(this->verbose)),

        cli::option("asynchronous-task").help("Active asynchronous-task logs")
        .action(cli::on_off_bit_location<RDPVerbose::asynchronous_task>(this->verbose)),

        cli::option("capabilities").help("Active capabilities logs")
        .action(cli::on_off_bit_location<RDPVerbose::capabilities>(this->verbose)),

        cli::option("rail").help("Active rail logs")
        .action(cli::on_off_bit_location<RDPVerbose::rail>(this->verbose)),

        cli::option("rail-dump").help("Actives rail logs and dump brute rail PDU")
        .action(cli::on_off_bit_location<RDPVerbose::rail_dump>(this->verbose)),


        cli::helper("========= Protocol ========="),

        cli::option("vnc").help("Set connection mod to VNC")
        .action([this](){
            this->mod_state = MOD_VNC;
            if (!bool(this->connection_info_cmd_complete & PORT_GOT)) {
                this->port = 5900;
            }
        }),

        cli::option("rdp").help("Set connection mod to RDP (default).")
        .action([this](){
            this->mod_state = MOD_RDP;
            this->port = 3389;
        }),

        cli::option("remote-app").help("Connection as remote application.")
        .action(cli::on_off_bit_location<MOD_RDP_REMOTE_APP>(this->mod_state)),

        cli::option("remote-exe").help("Connection as remote application and set the line command.")
        .action(cli::arg("command", [this](std::string line){
            this->mod_state = MOD_RDP_REMOTE_APP;
            this->modRDPParamsData.enable_shared_remoteapp = true;
            auto pos(line.find(' '));
            if (pos == std::string::npos) {
                this->rDPRemoteAppConfig.source_of_ExeOrFile = line;
                this->rDPRemoteAppConfig.source_of_Arguments.clear();
            }
            else {
                this->rDPRemoteAppConfig.source_of_ExeOrFile = line.substr(0, pos);
                this->rDPRemoteAppConfig.source_of_Arguments = line.substr(pos + 1);
            }
        })),

        cli::option("span").help("Span the screen size on local screen")
        .action(cli::on_off_location(this->is_spanning)),

        cli::option("enable-clipboard").help("Enable clipboard sharing")
        .action(cli::on_off_location(this->enable_shared_clipboard)),

        cli::option("enable-nla").help("Entable NLA protocol")
        .action(cli::on_off_location(this->modRDPParamsData.enable_nla)),

        cli::option("enable-tls").help("Enable TLS protocol")
        .action(cli::on_off_location(this->modRDPParamsData.enable_tls)),

        cli::option("enable-sound").help("Enable sound")
        .action(cli::on_off_location(this->modRDPParamsData.enable_sound)),

        cli::option("enable-fullwindowdrag").help("Enable full window draging")
        .action(cli::on_off_bit_location<~PERF_DISABLE_FULLWINDOWDRAG>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-menuanimations").help("Enable menu animations")
        .action(cli::on_off_bit_location<~PERF_DISABLE_MENUANIMATIONS>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-theming").help("Enable theming")
        .action(cli::on_off_bit_location<~PERF_DISABLE_THEMING>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-cursor-shadow").help("Enable cursor shadow")
        .action(cli::on_off_bit_location<~PERF_DISABLE_CURSOR_SHADOW>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-cursorsettings").help("Enable cursor settings")
        .action(cli::on_off_bit_location<~PERF_DISABLE_CURSORSETTINGS>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-font-smoothing").help("Enable font smoothing")
        .action(cli::on_off_bit_location<PERF_ENABLE_FONT_SMOOTHING>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-desktop-composition").help("Enable desktop composition")
        .action(cli::on_off_bit_location<PERF_ENABLE_DESKTOP_COMPOSITION>(
            this->info.rdp5_performanceflags)),

        cli::option("vnc-applekeyboard").help("Set keyboard compatibility mod with apple VNC server")
        .action(cli::on_off_location(this->vnc_conf.is_apple)),


        cli::option("keep_alive_frequence")
        .help("Set timeout to send keypress to keep the session alive")
        .action(cli::arg([&](int t){ keep_alive_freq = t; })),


        cli::helper("========= Client ========="),

        cli::option("width").help("Set screen width")
        .action(cli::arg_location(this->rdp_width)),

        cli::option("height").help("Set screen height")
        .action(cli::arg_location(this->rdp_height)),

        cli::option("bpp").help("Set bit per pixel (8, 15, 16, 24)")
        .action(cli::arg("bit_per_pixel", [this](int x) {
            this->info.screen_info.bpp = checked_int(x);
        })),

        cli::option("keylaout").help("Set windows keylayout")
        .action(cli::arg_location(this->info.keylayout)),

        cli::option("enable-record").help("Enable session recording as .wrm movie")
        .action(cli::on_off_location(this->is_recording)),

        cli::option("persist").help("Set connection to persist")
        .action([&]{
            quick_connection_test = false;
            persist = true;
        }),

        cli::option("timeout").help("Set timeout response before to disconnect in milisecond")
        .action(cli::arg("time", [&](long time){
            quick_connection_test = false;
            time_out_disconnection = std::chrono::milliseconds(time);
        })),

        cli::option("share-dir").help("Set directory path on local disk to share with your session.")
        .action(cli::arg("directory", [this](std::string s) {
            this->modRDPParamsData.enable_shared_virtual_disk = !s.empty();
            this->SHARE_DIR = std::move(s);
        })),

        cli::option("remote-dir").help("Remote working directory")
        .action(cli::arg_location("directory", this->rDPRemoteAppConfig.source_of_WorkingDir))
    );

    auto cli_result = cli::parse(options, argc, argv);
    switch (cli_result.res) {
        case cli::Res::Ok:
            break;
        case cli::Res::Exit:
            // TODO return 0;
            break;
        case cli::Res::Help:
            cli::print_help(options, std::cout);
            // TODO return 0;
            break;
        case cli::Res::BadFormat:
        case cli::Res::BadOption:
            std::cerr << "Bad " << (cli_result.res == cli::Res::BadFormat ? "format" : "option") << " at parameter " << cli_result.opti;
            if (cli_result.opti < cli_result.argc) {
                std::cerr << " (" << cli_result.argv[cli_result.opti] << ")";
            }
            std::cerr << "\n";
            // TODO return 1;
            break;
    }

    if (bool(RDPVerbose::rail & this->verbose)) {
        this->verbose = this->verbose | RDPVerbose::rail_order;
    }
}

// ~ClientRedemptionConfig() = default;

void ClientRedemptionConfig::set_icon_movie_data()
{
    this->icons_movie_data.clear();

    auto extension_av = ".mwrm"_av;

    if (DIR * dir = opendir(this->REPLAY_DIR.c_str())) {
        try {
            while (struct dirent * ent = readdir (dir)) {
                std::string current_name = ent->d_name;

                if (current_name.length() > extension_av.size()) {

                    std::string end_string(current_name.substr(
                        current_name.length()-extension_av.size(), current_name.length()));

                    if (end_string == extension_av.data()) {
                        std::string file_path = str_concat(this->REPLAY_DIR, '/', current_name);

                        unique_fd fd(file_path, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);

                        if(fd.is_open()){
                            std::string file_name(current_name.substr(0, current_name.length()-extension_av.size()));
                            std::string file_version;
                            std::string file_resolution;
                            std::string file_checksum;
                            long int movie_len = this->get_movie_time_length(file_path.c_str());

                            this->read_line(fd.fd(), file_version);
                            this->read_line(fd.fd(), file_resolution);
                            this->read_line(fd.fd(), file_checksum);

                            this->icons_movie_data.emplace_back(
                                std::move(file_name),
                                std::move(file_path),
                                std::move(file_version),
                                std::move(file_resolution),
                                std::move(file_checksum),
                                movie_len);

                        } else {
                            LOG(LOG_WARNING, "Can't open file \"%s\"", file_path);
                        }
                    }
                }
            }
        } catch (Error & e) {
            LOG(LOG_WARNING, "readdir error: (%u) %s", e.id, e.errmsg());
        }
        closedir (dir);
    }

    std::sort(this->icons_movie_data.begin(), this->icons_movie_data.end(), [](const IconMovieData& first, const IconMovieData& second) {
            return first.file_name < second.file_name;
        });
}

time_t ClientRedemptionConfig::get_movie_time_length(const char * mwrm_filename) {
    // TODO RZ: Support encrypted recorded file.

    CryptoContext cctx;
    Fstat fsats;
    InCryptoTransport trans(cctx, InCryptoTransport::EncryptionMode::NotEncrypted, fsats);
    MwrmReader mwrm_reader(trans);
    MetaLine meta_line;

    time_t start_time = 0;
    time_t stop_time = 0;

    trans.open(mwrm_filename);
    mwrm_reader.read_meta_headers();

    Transport::Read read_stat = mwrm_reader.read_meta_line(meta_line);

    if (read_stat == Transport::Read::Ok) {
        start_time = meta_line.start_time;
        stop_time = meta_line.stop_time;
        while (read_stat == Transport::Read::Ok) {
            stop_time = meta_line.stop_time;
            read_stat = mwrm_reader.read_meta_line(meta_line);
        }
    }

    return stop_time - start_time;
}

std::vector<IconMovieData> const& ClientRedemptionConfig::get_icon_movie_data() {

    this->set_icon_movie_data();

    return this->icons_movie_data;
}

void ClientRedemptionConfig::parse_options(int argc, char const* const argv[])
{
    auto options = cli::options(
        cli::helper("Client ReDemPtion Help menu."),

        cli::option('h', "help").help("Show help")
        .action(cli::help),

        cli::option('v', "version").help("Show version")
        .action(cli::quit([]{ std::cout << redemption_info_version() << "\n"; })),

        cli::helper("========= Connection ========="),

        cli::option('u', "username").help("Set target session user name")
        .action(cli::arg([this](std::string s){
            this->user_name = std::move(s);
            this->connection_info_cmd_complete += NAME_GOT;
        })),

        cli::option('p', "password").help("Set target session user password")
        .action(cli::arg([this](std::string s){
            this->user_password = std::move(s);
            this->connection_info_cmd_complete += PWD_GOT;
        })),

        cli::option('i', "ip").help("Set target IP address")
        .action(cli::arg([this](std::string s){
            this->target_IP = std::move(s);
            this->connection_info_cmd_complete += IP_GOT;
        })),

        cli::option('P', "port").help("Set port to use on target")
        .action(cli::arg([this](int n){
            this->port = n;
            this->connection_info_cmd_complete += PORT_GOT;
        })),


        cli::helper("========= Verbose ========="),

        cli::option("rdpdr").help("Active rdpdr logs")
        .action(cli::on_off_bit_location<RDPVerbose::rdpdr>(this->verbose)),

        cli::option("rdpsnd").help("Active rdpsnd logs")
        .action(cli::on_off_bit_location<RDPVerbose::rdpsnd>(this->verbose)),

        cli::option("cliprdr").help("Active cliprdr logs")
        .action(cli::on_off_bit_location<RDPVerbose::cliprdr>(this->verbose)),

        cli::option("graphics").help("Active graphics logs")
        .action(cli::on_off_bit_location<RDPVerbose::graphics>(this->verbose)),

        cli::option("printer").help("Active printer logs")
        .action(cli::on_off_bit_location<RDPVerbose::printer>(this->verbose)),

        cli::option("rdpdr-dump").help("Actives rdpdr logs and dump brute rdpdr PDU")
        .action(cli::on_off_bit_location<RDPVerbose::rdpdr_dump>(this->verbose)),

        cli::option("cliprd-dump").help("Actives cliprdr logs and dump brute cliprdr PDU")
        .action(cli::on_off_bit_location<RDPVerbose::cliprdr_dump>(this->verbose)),

        cli::option("basic-trace").help("Active basic-trace logs")
        .action(cli::on_off_bit_location<RDPVerbose::basic_trace>(this->verbose)),

        cli::option("connection").help("Active connection logs")
        .action(cli::on_off_bit_location<RDPVerbose::connection>(this->verbose)),

        cli::option("rail-order").help("Active rail-order logs")
        .action(cli::on_off_bit_location<RDPVerbose::rail_order>(this->verbose)),

        cli::option("asynchronous-task").help("Active asynchronous-task logs")
        .action(cli::on_off_bit_location<RDPVerbose::asynchronous_task>(this->verbose)),

        cli::option("capabilities").help("Active capabilities logs")
        .action(cli::on_off_bit_location<RDPVerbose::capabilities>(this->verbose)),

        cli::option("rail").help("Active rail logs")
        .action(cli::on_off_bit_location<RDPVerbose::rail>(this->verbose)),

        cli::option("rail-dump").help("Actives rail logs and dump brute rail PDU")
        .action(cli::on_off_bit_location<RDPVerbose::rail_dump>(this->verbose)),


        cli::helper("========= Protocol ========="),

        cli::option("vnc").help("Set connection mod to VNC")
        .action([this](){
            this->mod_state = MOD_VNC;
            if (!bool(this->connection_info_cmd_complete & PORT_GOT)) {
                this->port = 5900;
            }
        }),

        cli::option("rdp").help("Set connection mod to RDP (default).")
        .action([this](){ this->mod_state = MOD_VNC; }),

        cli::option("remote-app").help("Connection as remote application.")
        .action(cli::on_off_bit_location<MOD_RDP_REMOTE_APP>(this->mod_state)),

        cli::option("remote-exe").help("Connection as remote application and set the line command.")
        .action(cli::arg("command", [this](std::string line){
            this->mod_state = MOD_RDP_REMOTE_APP;
            auto pos(line.find(' '));
            if (pos == std::string::npos) {
                this->rDPRemoteAppConfig.source_of_ExeOrFile = line;
                this->rDPRemoteAppConfig.source_of_Arguments.clear();
            }
            else {
                this->rDPRemoteAppConfig.source_of_ExeOrFile = line.substr(0, pos);
                this->rDPRemoteAppConfig.source_of_Arguments = line.substr(pos + 1);
            }
        })),

        cli::option("span").help("Span the screen size on local screen")
        .action(cli::on_off_location(this->is_spanning)),

        cli::option("enable-clipboard").help("Enable clipboard sharing")
        .action(cli::on_off_location(this->enable_shared_clipboard)),

        cli::option("enable-nla").help("Entable NLA protocol")
        .action(cli::on_off_location(this->modRDPParamsData.enable_nla)),

        cli::option("enable-tls").help("Enable TLS protocol")
        .action(cli::on_off_location(this->modRDPParamsData.enable_tls)),

        cli::option("enable-sound").help("Enable sound")
        .action(cli::on_off_location(this->modRDPParamsData.enable_sound)),

        cli::option("enable-fullwindowdrag").help("Enable full window draging")
        .action(cli::on_off_bit_location<~PERF_DISABLE_FULLWINDOWDRAG>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-menuanimations").help("Enable menu animations")
        .action(cli::on_off_bit_location<~PERF_DISABLE_MENUANIMATIONS>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-theming").help("Enable theming")
        .action(cli::on_off_bit_location<~PERF_DISABLE_THEMING>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-cursor-shadow").help("Enable cursor shadow")
        .action(cli::on_off_bit_location<~PERF_DISABLE_CURSOR_SHADOW>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-cursorsettings").help("Enable cursor settings")
        .action(cli::on_off_bit_location<~PERF_DISABLE_CURSORSETTINGS>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-font-smoothing").help("Enable font smoothing")
        .action(cli::on_off_bit_location<PERF_ENABLE_FONT_SMOOTHING>(
            this->info.rdp5_performanceflags)),

        cli::option("enable-desktop-composition").help("Enable desktop composition")
        .action(cli::on_off_bit_location<PERF_ENABLE_DESKTOP_COMPOSITION>(
            this->info.rdp5_performanceflags)),

        cli::option("vnc-applekeyboard").help("Set keyboard compatibility mod with apple VNC server")
        .action(cli::on_off_location(this->vnc_conf.is_apple)),

        cli::helper("========= Client ========="),

        cli::option("width").help("Set screen width")
        .action(cli::arg_location(this->rdp_width)),

        cli::option("height").help("Set screen height")
        .action(cli::arg_location(this->rdp_height)),

        cli::option("bpp").help("Set bit per pixel (8, 15, 16, 24)")
        .action(cli::arg("bit_per_pixel", [this](int x) {
            this->info.screen_info.bpp = checked_int(x);
        })),

        cli::option("keylaout").help("Set windows keylayout")
        .action(cli::arg_location(this->info.keylayout)),

        cli::option("enable-record").help("Enable session recording as .wrm movie")
        .action(cli::on_off_location(this->is_recording)),

        cli::option("share-dir").help("Set directory path on local disk to share with your session.")
        .action(cli::arg("directory", [this](std::string s) {
            this->modRDPParamsData.enable_shared_virtual_disk = !s.empty();
            this->SHARE_DIR = std::move(s);
        })),

        cli::option("remote-dir").help("Remote directory")
        .action(cli::arg_location("directory", this->rDPRemoteAppConfig.source_of_WorkingDir)),

        cli::helper("========= Replay ========="),

        cli::option('R', "replay").help("Enable replay mode")
        .action(cli::on_off_location(this->is_full_replaying)),

        cli::option("replay-path").help("Set filename path for replay mode")
        .action(cli::arg([this](std::string s){
            this->is_full_replaying = true;
            this->full_capture_file_name = std::move(s);
        }))

    );

    auto cli_result = cli::parse(options, argc, argv);
    switch (cli_result.res) {
        case cli::Res::Ok:
            break;
        case cli::Res::Exit:
            // TODO return 0;
            break;
        case cli::Res::Help:
            cli::print_help(options, std::cout);
            // TODO return 0;
            break;
        case cli::Res::BadFormat:
        case cli::Res::BadOption:
            std::cerr << "Bad " << (cli_result.res == cli::Res::BadFormat ? "format" : "option") << " at parameter " << cli_result.opti;
            if (cli_result.opti < cli_result.argc) {
                std::cerr << " (";
                if (cli_result.res == cli::Res::BadFormat && cli_result.opti > 1) {
                    std::cerr << cli_result.argv[cli_result.opti-1] << " ";
                }
                std::cerr << cli_result.argv[cli_result.opti] << ")";
            }
            std::cerr << "\n";
            // TODO return 1;
            break;
    }
}

void ClientRedemptionConfig::openWindowsData()  {
    unique_fd file = unique_fd(this->WINDOWS_CONF.c_str(), O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

    if(file.is_open()) {
        this->windowsData.no_data = false;

        std::string line;
        int pos = 0;

        this->read_line(file.fd(), line);
        pos = line.find(' ');
        line = line.substr(pos, line.length());
        this->windowsData.form_x = std::stoi(line);

        this->read_line(file.fd(), line);
        pos = line.find(' ');
        line = line.substr(pos, line.length());
        this->windowsData.form_y = std::stoi(line);

        this->read_line(file.fd(), line);
        pos = line.find(' ');
        line = line.substr(pos, line.length());
        this->windowsData.screen_x = std::stoi(line);

        this->read_line(file.fd(), line);
        pos = line.find(' ');
        line = line.substr(pos, line.length());
        this->windowsData.screen_y = std::stoi(line);
    }
}

void ClientRedemptionConfig::writeWindowsData()
{
    this->windowsData.writeWindowsData();
}

void ClientRedemptionConfig::setUserProfil()  {
    unique_fd fd = unique_fd(this->USER_CONF_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if(fd.is_open()) {
        std::string line;
        this->read_line(fd.fd(), line);
        auto pos(line.find(' '));
        if (line.compare(0, pos, "current_user_profil_id") == 0) {
            this->current_user_profil = std::stoi(line.substr(pos + 1));
        }
    }
}

void ClientRedemptionConfig::setCustomKeyConfig()  {
    const std::string KEY_SETTING_PATH(this->MAIN_DIR + CLIENT_REDEMPTION_KEY_SETTING_PATH);
    unique_fd fd = unique_fd(KEY_SETTING_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);

    if (fd.is_open()) {
        this->keyCustomDefinitions.clear();

        std::string ligne;

        while(this->read_line(fd.fd(), ligne)) {

            int pos(ligne.find(' '));

            if (strcmp(ligne.substr(0, pos).c_str(), "-") == 0) {

                ligne = ligne.substr(pos + 1, ligne.length());
                pos = ligne.find(' ');

                int qtKeyID  = std::stoi(ligne.substr(0, pos));

                if (qtKeyID !=  0) {
                    ligne = ligne.substr(pos + 1, ligne.length());
                    pos = ligne.find(' ');

                    int scanCode = 0;
                    scanCode = std::stoi(ligne.substr(0, pos));
                    ligne = ligne.substr(pos + 1, ligne.length());

                    std::string ASCII8 = ligne.substr(0, 1);
                    int next_pos = 2;
                    if (ASCII8 == " ") {
                        ASCII8 = "";
                        next_pos = 1;
                    }

                    ligne = ligne.substr(next_pos, ligne.length());
                    int extended = std::stoi(ligne.substr(0, 1));

                    if (extended) {
                        extended = 1;
                    }
                    pos = ligne.find(' ');

                    std::string name = ligne.substr(pos + 1, ligne.length());

                    this->keyCustomDefinitions.emplace_back(qtKeyID, scanCode, ASCII8, extended, name);
                }
            }
        }
    }
}

void ClientRedemptionConfig::writeCustomKeyConfig()  {
    const std::string KEY_SETTING_PATH(this->MAIN_DIR + CLIENT_REDEMPTION_KEY_SETTING_PATH);
    unique_fd fd = unique_fd(KEY_SETTING_PATH.c_str(), O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

    if(fd.is_open()) {
        std::string to_write = "Key Setting\n\n";

        for (KeyCustomDefinition & key : this->keyCustomDefinitions) {
            if (key.qtKeyID != 0) {
                str_append(
                    to_write,
                    "- ",
                    std::to_string(key.qtKeyID), ' ',
                    std::to_string(key.scanCode), ' ',
                    key.ASCII8, ' ',
                    std::to_string(key.extended), ' ',
                    key.name, '\n'
                );
            }
        }

        int res = ::write(fd.fd(), to_write.c_str(), to_write.length());
        // TODO: partial write is unlikely but may occur sometimes and should be managed!
        if (res < to_write.length()){
            std::cerr << "Write of Custom key config to " << KEY_SETTING_PATH << " failed.";
        }
    }
    else {
        std::cerr << "Open of " << KEY_SETTING_PATH << " to write Key Settings failed.";
    }
}


void ClientRedemptionConfig::add_key_custom_definition(int qtKeyID, int scanCode, const std::string & ASCII8, int extended, const std::string & name)  {
    this->keyCustomDefinitions.emplace_back(qtKeyID, scanCode, ASCII8, extended, name);
}

void ClientRedemptionConfig::setClientInfo()  {
    this->userProfils.clear();
    this->userProfils.emplace_back(0, "Default");

    unique_fd fd = unique_fd(this->USER_CONF_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);

    if (fd.is_open()) {

        int read_id(-1);
        std::string line;

        while(this->read_line(fd.fd(), line)) {

            auto pos(line.find(' '));
            std::string info = line.substr(pos + 1);
            std::string tag = line.substr(0, pos);
            if (tag == "id") {
                read_id = std::stoi(info);
            } else
            if (tag == "name") {
                if (read_id) {
                    this->userProfils.emplace_back(read_id, info);
                }
            } else
            if (this->current_user_profil == read_id) {

                if (tag == "keylayout") {
                    this->info.keylayout = std::stoi(info);
                } else
                if (tag == "console_session") {
                    this->info.console_session = std::stoi(info);
                } else
                if (tag == "brush_cache_code") {
                    this->info.brush_cache_code = std::stoi(info);
                } else
                if (tag == "bpp") {
                    this->info.screen_info.bpp = checked_int(std::stoi(info));
                } else
                if (tag == "width") {
                    this->rdp_width     = std::stoi(info);
                } else
                if (tag == "height") {
                    this->rdp_height     = std::stoi(info);
                } else
                if (tag == "monitorCount") {
                    this->info.cs_monitor.monitorCount = std::stoi(info);
                } else
                if (tag == "span") {
                    if (std::stoi(info)) {
                        this->is_spanning = true;
                    } else {
                        this->is_spanning = false;
                    }
                } else
                if (tag == "record") {
                    if (std::stoi(info)) {
                        this->is_recording = true;
                    } else {
                        this->is_recording = false;
                    }
                } else
                if (tag == "tls") {
                    this->modRDPParamsData.enable_tls = bool(std::stoi(info));
                } else
                if (tag == "nla") {
                    this->modRDPParamsData.enable_nla = bool(std::stoi(info));
                } else
                if (tag == "sound") {
                    this->modRDPParamsData.enable_sound = bool(std::stoi(info));
                } else
                if (tag == "console_mode") {
                    this->info.console_session = (std::stoi(info) > 0);
                } else
                if (tag == "enable_shared_clipboard") {
                    this->enable_shared_clipboard = bool(std::stoi(info));
                } else
                if (tag == "enable_shared_remoteapp") {
                        this->modRDPParamsData.enable_shared_remoteapp = bool(std::stoi(info));
                } else
                if (tag == "enable_shared_virtual_disk") {
                    this->modRDPParamsData.enable_shared_virtual_disk = bool(std::stoi(info));
                } else
                if (tag == "share-dir") {
                    this->SHARE_DIR                 = info;
                } else
                if (tag == "remote-exe") {
                    this->rDPRemoteAppConfig.full_cmd_line                = info;
                    auto arfs_pos(info.find(' '));
                    if (arfs_pos == 0) {
                        this->rDPRemoteAppConfig.source_of_ExeOrFile = info;
                        this->rDPRemoteAppConfig.source_of_Arguments.clear();
                    }
                    else {
                        this->rDPRemoteAppConfig.source_of_ExeOrFile = info.substr(0, arfs_pos);
                        this->rDPRemoteAppConfig.source_of_Arguments = info.substr(arfs_pos + 1);
                    }
                } else
                if (tag == "remote-dir") {
                    this->rDPRemoteAppConfig.source_of_WorkingDir                = info;
                } else
                if (tag == "rdp5_performanceflags") {
                    this->info.rdp5_performanceflags = std::stoi(info);
                } else
                if (tag == "vnc-applekeyboard ") {
                    this->vnc_conf.is_apple = bool(std::stoi(info));
                } else
                if (tag == "mod") {
                    this->mod_state = std::stoi(info);

                    read_id = -1;
                }
            }

            line = "";
        }
    }
}

bool ClientRedemptionConfig::read_line(const int fd, std::string & line) {
    line = "";
    if (fd < 0) {
        return false;
    }
    char c[2] = {'\0', '\0'};
    int size = -1;
    while (c[0] != '\n' && size !=  0) {
        size_t size = ::read(fd, c, 1);
        if (size == 1) {
            if (c[0] == '\n') {
                return true;
            } else {
                line += c[0];
            }
        } else {
            return false;
        }
    }
    return false;
}

void ClientRedemptionConfig::setAccountData()  {
    this->_accountNB = 0;

    unique_fd fd = unique_fd(this->USER_CONF_LOG.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);

    if (fd.is_open()) {
        int accountNB(0);
        std::string line;

        while(read_line(fd.fd(), line)) {
            auto pos(line.find(' '));
            std::string info = line.substr(pos + 1);

            if (line.compare(0, pos, "save_pwd") == 0) {
                this->_save_password_account = (info == "true");
            } else
            if (line.compare(0, pos, "last_target") == 0) {
                this->_last_target_index = std::stoi(info);
            } else
            if (line.compare(0, pos, "title") == 0) {
                AccountData new_account;
                this->_accountData.push_back(new_account);
                this->_accountData.back().title = info;
            } else
            if (line.compare(0, pos, "IP") == 0) {
                this->_accountData.back().IP = info;
            } else
            if (line.compare(0, pos, "name") == 0) {
                this->_accountData.back().name = info;
            } else if (line.compare(0, pos, "protocol") == 0) {
                this->_accountData.back().protocol = std::stoi(info);
            } else
            if (line.compare(0, pos, "pwd") == 0) {
                this->_accountData.back().pwd = info;
            } else
            if (line.compare(0, pos, "options_profil") == 0) {

                this->_accountData.back().options_profil = std::stoi(info);
                this->_accountData.back().index = accountNB;

                accountNB++;
                if (accountNB == MAX_ACCOUNT_DATA) {
                    this->_accountNB = MAX_ACCOUNT_DATA;
                    accountNB = 0;
                }
            } else
            if (line.compare(0, pos, "port") == 0) {
                this->_accountData.back().port = std::stoi(info);
            }

            line = "";
        }

        if (this->_accountNB < MAX_ACCOUNT_DATA) {
            this->_accountNB = accountNB;
        }

//         if (this->_last_target_index < this->_accountData.size()) {
//
//             this->target_IP = this->_accountData[this->_last_target_index].IP;
//             this->user_name = this->_accountData[this->_last_target_index].name;
//             this->user_password = this->_accountData[this->_last_target_index].pwd;
//             this->port = this->_accountData[this->_last_target_index].port;
//         }
    }
}



void ClientRedemptionConfig::writeAccoundData(const std::string& ip, const std::string& name, const std::string& pwd, const int port)  {
    if (this->connected) {
        bool alreadySet = false;

        std::string title(ip + " - " + name);

        for (int i = 0; i < this->_accountNB; i++) {
            if (this->_accountData[i].title == title) {
                alreadySet = true;
                this->_last_target_index = i;
                this->_accountData[i].pwd  = pwd;
                this->_accountData[i].port = port;
                this->_accountData[i].options_profil  = this->current_user_profil;
            }
        }

        if (!alreadySet && (this->_accountNB < MAX_ACCOUNT_DATA)) {
            AccountData new_account;
            this->_accountData.push_back(new_account);
            this->_accountData[this->_accountNB].title = title;
            this->_accountData[this->_accountNB].IP    = ip;
            this->_accountData[this->_accountNB].name  = name;
            this->_accountData[this->_accountNB].pwd   = pwd;
            this->_accountData[this->_accountNB].port  = port;
            this->_accountData[this->_accountNB].options_profil  = this->current_user_profil;
            this->_accountData[this->_accountNB].protocol = this->mod_state;
            this->_accountNB++;

            if (this->_accountNB > MAX_ACCOUNT_DATA) {
                this->_accountNB = MAX_ACCOUNT_DATA;
            }
            this->_last_target_index = this->_accountNB;
        }

        unique_fd file = unique_fd(this->USER_CONF_LOG.c_str(), O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
        if(file.is_open()) {

            std::string to_write = str_concat(
                (this->_save_password_account ? "save_pwd true\n" : "save_pwd false\n"),
                "last_target ",
                std::to_string(this->_last_target_index),
                "\n\n");

            for (int i = 0; i < this->_accountNB; i++) {
                str_append(
                    to_write,
                    "title ", this->_accountData[i].title, "\n"
                    "IP "   , this->_accountData[i].IP   , "\n"
                    "name " , this->_accountData[i].name , "\n"
                    "protocol ", std::to_string(this->_accountData[i].protocol), '\n');

                if (this->_save_password_account) {
                    str_append(to_write, "pwd ", this->_accountData[i].pwd, "\n");
                } else {
                    to_write += "pwd \n";
                }

                str_append(
                    to_write,
                    "port ", std::to_string(this->_accountData[i].port), "\n"
                    "options_profil ", std::to_string(this->_accountData[i].options_profil), "\n"
                    "\n");
            }

            int res = ::write(file.fd(), to_write.c_str(), to_write.length());
            // TODO: partial write is unlikely but may occur sometimes and should be managed!
            if (res < to_write.length()){
                std::cerr << "Write of Custom key config to " << this->USER_CONF_LOG << " failed.";
            }

        }
    }
}

void ClientRedemptionConfig::set_remoteapp_cmd_line(const std::string & cmd)  {
    this->rDPRemoteAppConfig.full_cmd_line = cmd;
    int pos = cmd.find(' ');
    this->rDPRemoteAppConfig.source_of_ExeOrFile = cmd.substr(0, pos);
    this->rDPRemoteAppConfig.source_of_Arguments = cmd.substr(pos + 1);
}

bool ClientRedemptionConfig::is_no_win_data()  {
    return this->windowsData.no_data;
}

void ClientRedemptionConfig::deleteCurrentProtile()  {
    unique_fd file_to_read = unique_fd(this->USER_CONF_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if(file_to_read.is_open()) {

        std::string new_file_content = "current_user_profil_id 0\n";
        int ligne_to_jump = 0;

        std::string line;

        this->read_line(file_to_read.fd(), line);

        while(this->read_line(file_to_read.fd(), line)) {
            if (ligne_to_jump == 0) {
                int pos = line.find(' ');
                std::string info = line.substr(pos + 1);

                if (line.compare(0, pos, "id") == 0 && std::stoi(info) == this->current_user_profil) {
                    ligne_to_jump = 18;
                } else {
                    str_append(new_file_content, line, '\n');
                }
            } else {
                ligne_to_jump--;
            }
        }

        file_to_read.close();

        unique_fd file_to_read = unique_fd(this->USER_CONF_PATH.c_str(), O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        int res = ::write(file_to_read.fd(), new_file_content.c_str(), new_file_content.length());
        if (res < new_file_content.length()){
            std::cerr << "Deletion of current profile in " << this->USER_CONF_PATH << " failed.";
        }

    }
}


void ClientRedemptionConfig::setDefaultConfig()  {
    //this->current_user_profil = 0;
    this->info.keylayout = 0x040C;// 0x40C FR, 0x409 USA
    this->info.brush_cache_code = 0;
    this->info.screen_info.bpp = BitsPerPixel{24};
    this->info.screen_info.width  = 800;
    this->info.screen_info.height = 600;
    this->info.console_session = false;
    this->info.rdp5_performanceflags = 0;               //PERF_DISABLE_WALLPAPER;
    this->info.cs_monitor.monitorCount = 1;
    this->is_spanning = false;
    this->is_recording = false;
    this->modRDPParamsData.enable_tls = true;
    this->modRDPParamsData.enable_nla = true;
    this->enable_shared_clipboard = true;
    this->modRDPParamsData.enable_shared_virtual_disk = true;
    this->SHARE_DIR = "/home";
    //this->info.encryptionLevel = 1;
}


void ClientRedemptionConfig::writeClientInfo()  {

    unique_fd file = unique_fd(this->USER_CONF_PATH.c_str(), O_WRONLY| O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

    if(file.is_open()) {

        std::string to_write = str_concat(
            "current_user_profil_id ", std::to_string(this->current_user_profil), '\n');

        bool not_reading_current_profil = true;
        std::string ligne;
        while(this->read_line(file.fd(), ligne)) {
            if (!ligne.empty()) {
                std::size_t pos = ligne.find(' ');

                if (ligne.compare(pos+1, std::string::npos, "id") == 0) {
                    to_write += '\n';
                    int read_id = std::stoi(ligne);
                    not_reading_current_profil = !(read_id == this->current_user_profil);
                }

                if (not_reading_current_profil) {
                    str_append(to_write, '\n', ligne);
                }

                ligne.clear();
            }
        }

        str_append(
            to_write,
            "\nid ", std::to_string(this->userProfils[this->current_user_profil].id), "\n"
            "name ", this->userProfils[this->current_user_profil].name, "\n"
            "keylayout ", std::to_string(this->info.keylayout), "\n"
            "brush_cache_code ", std::to_string(this->info.brush_cache_code), "\n"
            "bpp ", std::to_string(static_cast<int>(this->info.screen_info.bpp)), "\n"
            "width ", std::to_string(this->rdp_width), "\n"
            "height ", std::to_string(this->rdp_height), "\n"
            "rdp5_performanceflags ", std::to_string(static_cast<int>(this->info.rdp5_performanceflags)), "\n"
            "monitorCount ", std::to_string(this->info.cs_monitor.monitorCount), "\n"
            "span ", std::to_string(this->is_spanning), "\n"
            "record ", std::to_string(this->is_recording),"\n"
            "tls ", std::to_string(this->modRDPParamsData.enable_tls), "\n"
            "nla ", std::to_string(this->modRDPParamsData.enable_nla), "\n"
            "sound ", std::to_string(this->modRDPParamsData.enable_sound), "\n"
            "console_mode ", std::to_string(this->info.console_session), "\n"
            "enable_shared_clipboard ", std::to_string(this->enable_shared_clipboard), "\n"
            "enable_shared_virtual_disk ", std::to_string(this->modRDPParamsData.enable_shared_virtual_disk), "\n"
            "enable_shared_remoteapp ", std::to_string(this->modRDPParamsData.enable_shared_remoteapp), "\n"
            "share-dir ", this->SHARE_DIR, "\n"
            "remote-exe ", this->rDPRemoteAppConfig.full_cmd_line, "\n"
            "remote-dir ", this->rDPRemoteAppConfig.source_of_WorkingDir, "\n"
            "vnc-applekeyboard ", std::to_string(this->vnc_conf.is_apple), "\n"
            "mod ", std::to_string(static_cast<int>(this->mod_state)) , "\n"
        );

        ::write(file.fd(), to_write.c_str(), to_write.length());
    }
}

