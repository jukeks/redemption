#include "font.hpp"
#include "config_types.hpp"
#include "config_includes.hpp"

namespace cfg {
    struct theme {
        static constexpr ::configs::VariableProperties properties() {
            return ::configs::VariableProperties::none;
        }
        using type = Theme;
        type value{};
    };
    struct font {
        static constexpr ::configs::VariableProperties properties() {
            return ::configs::VariableProperties::none;
        }
        using type = Font;
        font(char const * filename) : value(filename) {}
        type value;
    };

    struct client {
        // AUTHID_KEYBOARD_LAYOUT
        struct keyboard_layout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 19; }
            using type = unsigned;
            type value{0};
        };
        // If true, ignore password provided by RDP client, user need do login manually.
        struct ignore_logon_password {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        struct performance_flags_default {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        // Disable theme (0x8).
        struct performance_flags_force_present {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{8};
        };
        // Disable font smoothing (0x80).
        struct performance_flags_force_not_present {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{128};
        };

        // Fallback to RDP Legacy Encryption if client does not support TLS.
        struct tls_fallback_legacy {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        struct tls_support {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        // Needed to connect with jrdp, based on bogus X224 layer code.
        struct bogus_neg_request {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        // Needed to connect with Remmina 0.8.3 and freerdp 0.9.4, based on bogus MCS layer code.
        struct bogus_user_id {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };

        // If enabled, ignore CTRL+ALT+DEL and CTRL+SHIFT+ESCAPE (or the equivalents) keyboard sequences.
        // AUTHID_DISABLE_TSK_SWITCH_SHORTCUTS
        struct disable_tsk_switch_shortcuts {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 20; }
            using type = bool;
            type value{0};
        };

        // Specifies the highest compression package support available on the front side:
        //   0: the RDP bulk compression is disabled
        //   1: RDP 4.0 bulk compression
        //   2: RDP 5.0 bulk compression
        //   3: RDP 6.0 bulk compression
        //   4: RDP 6.1 bulk compression
        struct rdp_compression {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Range<unsigned, 0, 4, 0>;
            type value{4};
        };

        // Specifies the maximum color resolution (color depth) for client session:
        //   8: 8 bbp
        //   15: 15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
        //   16: 16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
        //   24: 24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)
        struct max_color_depth {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::ColorDepth;
            type value{static_cast< ::configs::ColorDepth>(24)};
        };

        // Persistent Disk Bitmap Cache on the front side.
        struct persistent_disk_bitmap_cache {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        // Support of Cache Waiting List (this value is ignored if Persistent Disk Bitmap Cache is disabled).
        struct cache_waiting_list {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        // If enabled, the contents of Persistent Bitmap Caches are stored on disk.
        struct persist_bitmap_cache_on_disk {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        // Support of Bitmap Compression.
        struct bitmap_compression {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };

        // Enables support of Clent Fast-Path Input Event PDUs.
        struct fast_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
    };

    struct context {
        struct movie {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticString<1024>;
            type value{};
        };

        // AUTHID_OPT_BITRATE
        struct opt_bitrate {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 33; }
            using type = unsigned;
            type value{40000};
        };
        // AUTHID_OPT_FRAMERATE
        struct opt_framerate {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 34; }
            using type = unsigned;
            type value{5};
        };
        // AUTHID_OPT_QSCALE
        struct opt_qscale {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 35; }
            using type = unsigned;
            type value{15};
        };

        // AUTHID_OPT_BPP
        struct opt_bpp {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 36; }
            using type = unsigned;
            type value{24};
        };
        // AUTHID_OPT_HEIGHT
        struct opt_height {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 37; }
            using type = unsigned;
            type value{600};
        };
        // AUTHID_OPT_WIDTH
        struct opt_width {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 38; }
            using type = unsigned;
            type value{800};
        };

        // auth_error_message is left as std::string type
        // because SocketTransport and ReplayMod take it as argument on
        // constructor and modify it as a std::string
        // AUTHID_AUTH_ERROR_MESSAGE
        struct auth_error_message {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 39; }
            using type = std::string;
            type value{};
        };

        // AUTHID_SELECTOR
        struct selector {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 40; }
            using type = bool;
            type value{0};
        };
        // AUTHID_SELECTOR_CURRENT_PAGE
        struct selector_current_page {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 41; }
            using type = unsigned;
            type value{1};
        };
        // AUTHID_SELECTOR_DEVICE_FILTER
        struct selector_device_filter {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 42; }
            using type = std::string;
            type value{};
        };
        // AUTHID_SELECTOR_GROUP_FILTER
        struct selector_group_filter {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 43; }
            using type = std::string;
            type value{};
        };
        // AUTHID_SELECTOR_PROTO_FILTER
        struct selector_proto_filter {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 44; }
            using type = std::string;
            type value{};
        };
        // AUTHID_SELECTOR_LINES_PER_PAGE
        struct selector_lines_per_page {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 45; }
            using type = unsigned;
            type value{0};
        };
        // AUTHID_SELECTOR_NUMBER_OF_PAGES
        struct selector_number_of_pages {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 46; }
            using type = unsigned;
            type value{1};
        };

        // AUTHID_TARGET_PASSWORD
        struct target_password {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 47; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET_HOST
        struct target_host {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 48; }
            using type = std::string;
            type value{""};
        };
        // AUTHID_TARGET_SERVICE
        struct target_service {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 49; }
            using type = std::string;
            type value{""};
        };
        // AUTHID_TARGET_PORT
        struct target_port {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 50; }
            using type = unsigned;
            type value{3389};
        };
        // AUTHID_TARGET_PROTOCOL
        struct target_protocol {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 51; }
            using type = std::string;
            type value{"RDP"};
        };

        // AUTHID_PASSWORD
        struct password {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 52; }
            using type = std::string;
            type value{};
        };

        // AUTHID_REPORTING
        struct reporting {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 53; }
            using type = std::string;
            type value{};
        };

        // AUTHID_AUTH_CHANNEL_ANSWER
        struct auth_channel_answer {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 54; }
            using type = std::string;
            type value{};
        };
        // AUTHID_AUTH_CHANNEL_RESULT
        struct auth_channel_result {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 55; }
            using type = std::string;
            type value{};
        };
        // AUTHID_AUTH_CHANNEL_TARGET
        struct auth_channel_target {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 56; }
            using type = std::string;
            type value{};
        };

        // AUTHID_MESSAGE
        struct message {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 57; }
            using type = std::string;
            type value{};
        };

        TODO("why are the field below Strings ? They should be booleans. As they can only contain True/False to know if a user clicked on a button")
        // AUTHID_ACCEPT_MESSAGE
        struct accept_message {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 58; }
            using type = std::string;
            type value{};
        };
        // AUTHID_DISPLAY_MESSAGE
        struct display_message {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 59; }
            using type = std::string;
            type value{};
        };

        // AUTHID_REJECTED
        struct rejected {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 60; }
            using type = std::string;
            type value{};
        };

        // AUTHID_AUTHENTICATED
        struct authenticated {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 61; }
            using type = bool;
            type value{0};
        };

        // AUTHID_KEEPALIVE
        struct keepalive {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 62; }
            using type = bool;
            type value{0};
        };

        // AUTHID_SESSION_ID
        struct session_id {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 63; }
            using type = std::string;
            type value{};
        };

        // AUTHID_END_DATE_CNX
        struct end_date_cnx {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 64; }
            using type = unsigned;
            type value{0};
        };
        // AUTHID_END_TIME
        struct end_time {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 65; }
            using type = std::string;
            type value{};
        };

        // AUTHID_MODE_CONSOLE
        struct mode_console {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 66; }
            using type = std::string;
            type value{"allow"};
        };
        // AUTHID_TIMEZONE
        struct timezone {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 67; }
            using type = int;
            type value{-3600};
        };

        // AUTHID_REAL_TARGET_DEVICE
        struct real_target_device {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 68; }
            using type = std::string;
            type value{};
        };

        // AUTHID_AUTHENTICATION_CHALLENGE
        struct authentication_challenge {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 69; }
            using type = bool;
            type value{};
        };

        // AUTHID_TICKET
        struct ticket {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 70; }
            using type = std::string;
            type value{""};
        };
        // AUTHID_COMMENT
        struct comment {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 71; }
            using type = std::string;
            type value{""};
        };
        // AUTHID_DURATION
        struct duration {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 72; }
            using type = std::string;
            type value{""};
        };
        // AUTHID_WAITINFORETURN
        struct waitinforeturn {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 73; }
            using type = std::string;
            type value{""};
        };
        // AUTHID_SHOWFORM
        struct showform {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 74; }
            using type = bool;
            type value{0};
        };
        // AUTHID_FORMFLAG
        struct formflag {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 75; }
            using type = unsigned;
            type value{0};
        };

        // AUTHID_MODULE
        struct module {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 76; }
            using type = std::string;
            type value{"login"};
        };
        // AUTHID_FORCEMODULE
        struct forcemodule {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 77; }
            using type = bool;
            type value{0};
        };
        // AUTHID_PROXY_OPT
        struct proxy_opt {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 78; }
            using type = std::string;
            type value{};
        };
    };

    struct crypto {
        struct key0 {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticKeyString<32>;
            type value{"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"};
        };
        struct key1 {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticKeyString<32>;
            type value{"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"};
        };
    };

    struct debug {
        struct x224 {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct mcs {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct sec {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct rdp {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct primary_orders {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct secondary_orders {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct bitmap {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct capture {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct auth {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct session {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct front {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct mod_rdp {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct mod_vnc {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct mod_int {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct mod_xup {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct widget {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct input {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct password {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct compression {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct cache {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct bitmap_update {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct performance {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };
        struct pass_dialog_box {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };

        struct config {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Range<unsigned, 0, 2, 0>;
            type value{2};
        };
    };

    struct globals {
        // AUTHID_CAPTURE_CHUNK
        struct capture_chunk {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 0; }
            using type = bool;
            type value{};
        };

        // AUTHID_AUTH_USER
        struct auth_user {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 1; }
            using type = std::string;
            type value{};
        };
        // AUTHID_HOST
        struct host {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 2; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET
        struct target {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 3; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET_DEVICE
        struct target_device {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 4; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET_USER
        struct target_user {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 5; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET_APPLICATION
        struct target_application {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 6; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET_APPLICATION_ACCOUNT
        struct target_application_account {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 7; }
            using type = std::string;
            type value{};
        };
        // AUTHID_TARGET_APPLICATION_PASSWORD
        struct target_application_password {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 8; }
            using type = std::string;
            type value{};
        };

        // Support of Bitmap Cache.
        struct bitmap_cache {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        struct glyph_cache {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        struct port {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{3389};
        };
        struct nomouse {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        struct notimestamp {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        // low, medium or high.
        struct encryptionLevel {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Level;
            type value{static_cast< ::configs::Level>(0)};
        };
        struct authip {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticIpString;
            type value{"127.0.0.1"};
        };
        struct authport {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{3350};
        };

        // No traffic auto disconnection (in seconds).
        struct session_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{900};
        };
        // Keepalive (in seconds).
        struct keepalive_grace_delay {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{30};
        };
        // Specifies the time to spend on the close box of proxy RDP before closing client window (0 to desactivate).
        struct close_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{600};
        };

        // Authentication channel used by Auto IT scripts. May be '*' to use default name. Keep empty to disable virtual channel.
        struct auth_channel {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticNilString<8>;
            type value{::configs::null_fill()};
        };
        // AUTHID_OPT_FILE_ENCRYPTION
        struct enable_file_encryption {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read | ::configs::VariableProperties::write;
            }
            static constexpr unsigned index() { return 9; }
            using type = bool;
            type value{};
        };
        struct listen_address {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticIpString;
            type value{"0.0.0.0"};
        };
        // Allow IP Transparent.
        struct enable_ip_transparent {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        // Proxy certificate password.
        struct certificate_password {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticString<256>;
            type value{"inquisition"};
        };

        struct png_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticString<1024>;
            type value{PNG_PATH};
        };
        struct wrm_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticString<1024>;
            type value{WRM_PATH};
        };

        // AUTHID_ALTERNATE_SHELL
        struct alternate_shell {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 10; }
            using type = std::string;
            type value{};
        };
        // AUTHID_SHELL_WORKING_DIRECTORY
        struct shell_working_directory {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 11; }
            using type = std::string;
            type value{};
        };

        // AUTHID_OPT_MOVIE
        struct movie {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 12; }
            using type = bool;
            type value{0};
        };
        // AUTHID_OPT_MOVIE_PATH
        struct movie_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 13; }
            using type = std::string;
            type value{};
        };
        // Support of Bitmap Update.
        struct enable_bitmap_update {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };

        // Show close screen.
        struct enable_close_box {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        struct enable_osd {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        struct enable_osd_display_remote_target {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };

        // AUTHID_OPT_WABAGENT
        struct enable_wab_agent {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 14; }
            using type = bool;
            type value{0};
        };
        // AUTHID_ENABLE_WAB_AGENT_LOADING_MASK
        struct enable_wab_agent_loading_mask {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 15; }
            using type = bool;
            type value{1};
        };
        // AUTHID_OPT_WABAGENT_LAUNCH_TIMEOUT
        struct wab_agent_launch_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 16; }
            using type = unsigned;
            type value{20000};
        };
        // Specifies the action to be performed is the launch of agent fails.
        //   0: disconnects session
        //   1: remains connected
        // AUTHID_OPT_WABAGENT_ON_LAUNCH_FAILURE
        struct wab_agent_on_launch_failure {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 17; }
            using type = ::configs::Range<unsigned, 0, 1, 0>;
            type value{0};
        };
        // AUTHID_OPT_WABAGENT_KEEPALIVE_TIMEOUT
        struct wab_agent_keepalive_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 18; }
            using type = unsigned;
            type value{5000};
        };

        struct wab_agent_alternate_shell {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticString<512>;
            type value{""};
        };

        struct persistent_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticPath<1024>;
            type value{PERSISTENT_PATH};
        };

        struct disable_proxy_opt {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
    };

    struct internal_mod {
        struct theme {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = std::string;
            type value{""};
        };
    };

    struct mod_rdp {
        // Specifies the highest compression package support available on the front side:
        //   0: the RDP bulk compression is disabled
        //   1: RDP 4.0 bulk compression
        //   2: RDP 5.0 bulk compression
        //   3: RDP 6.0 bulk compression
        //   4: RDP 6.1 bulk compression
        struct rdp_compression {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Range<unsigned, 0, 4, 0>;
            type value{4};
        };

        struct disconnect_on_logon_user_change {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        struct open_session_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = uint32_t;
            type value{0};
        };

        // 0: Cancel connection and reports error.
        // 1: Replace existing certificate and continue connection.
        struct certificate_change_action {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Range<unsigned, 0, 1, 0>;
            type value{1};
        };

        // Enables support of additional drawing orders:
        //   15: MultiDstBlt
        //   16: MultiPatBlt
        //   17: MultiScrBlt
        //   18: MultiOpaqueRect
        //   22: Polyline
        struct extra_orders {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = std::string;
            type value{"15,16,17,18,22"};
        };

        // NLA authentication in secondary target.
        struct enable_nla {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        // If enabled, NLA authentication will try Kerberos before NTLM.
        // (if enable_nla is disabled, this value is ignored).
        struct enable_kerberos {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        // Persistent Disk Bitmap Cache on the mod side.
        struct persistent_disk_bitmap_cache {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        // Support of Cache Waiting List (this value is ignored if Persistent Disk Bitmap Cache is disabled).
        struct cache_waiting_list {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };
        // If enabled, the contents of Persistent Bitmap Caches are stored on disk.
        struct persist_bitmap_cache_on_disk {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        // Enables channels names (example: channel1,channel2,etc). Character * only, activate all with low priority.
        struct allow_channels {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = std::string;
            type value{"*"};
        };
        // Disable channels names (example: channel1,channel2,etc). Character * only, deactivate all with low priority.
        struct deny_channels {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = std::string;
            type value{};
        };

        // Enables support of Server Fast-Path Update PDUs.
        struct fast_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{1};
        };

        // Enables Server Redirection Support.
        struct server_redirection_support {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        struct redir_info {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = RedirectionInfo;
            type value{};
        };

        // Needed to connect with VirtualBox, based on bogus TS_UD_SC_NET data block.
        // AUTHID_RDP_BOGUS_SC_NET_SIZE
        struct bogus_sc_net_size {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 21; }
            using type = bool;
            type value{1};
        };

        // AUTHID_OPT_CLIENT_DEVICE_ANNOUNCE_TIMEOUT
        struct client_device_announce_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 22; }
            using type = unsigned;
            type value{1000};
        };

        // AUTHID_OPT_PROXY_MANAGED_DRIVES
        struct proxy_managed_drives {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 23; }
            using type = std::string;
            type value{};
        };
    };

    struct mod_replay {
        // 0 - Wait for Escape, 1 - End session
        struct on_end_of_data {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
    };

    struct mod_vnc {
        // Enable or disable the clipboard from client (client to server).
        // AUTHID_VNC_CLIPBOARD_UP
        struct clipboard_up {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 24; }
            using type = bool;
            type value{};
        };
        // Enable or disable the clipboard from server (server to client).
        // AUTHID_VNC_CLIPBOARD_DOWN
        struct clipboard_down {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 25; }
            using type = bool;
            type value{};
        };

        // Sets the encoding types in which pixel data can be sent by the VNC server:
        //   0: Raw
        //   1: CopyRect
        //   2: RRE
        //   16: ZRLE
        //   -239 (0xFFFFFF11): Cursor pseudo-encoding
        struct encodings {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = std::string;
            type value{};
        };

        struct allow_authentification_retries {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };

        // VNC server clipboard data encoding type.
        //   latin1 (default) or utf-8
        // AUTHID_VNC_SERVER_CLIPBOARD_ENCODING_TYPE
        struct server_clipboard_encoding_type {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 26; }
            using type = ::configs::ClipboardEncodingType;
            type value{static_cast< ::configs::ClipboardEncodingType>(1)};
        };

        // AUTHID_VNC_BOGUS_CLIPBOARD_INFINITE_LOOP
        struct bogus_clipboard_infinite_loop {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 27; }
            using type = ::configs::Range<unsigned, 0, 2, 0>;
            type value{0};
        };
    };

    struct translation {
        // AUTHID_LANGUAGE
        struct language {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 32; }
            using type = ::configs::Language;
            type value{static_cast< ::configs::Language>(0)};
        };
    };

    struct video {
        struct capture_groupid {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{33};
        };

        // Specifies the type of data to be captured:
        //   1: PNG
        //   2: WRM
        struct capture_flags {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::CaptureFlags;
            type value{static_cast< ::configs::CaptureFlags>(3)};
        };

        // Frame interval is in 1/10 s.
        struct png_interval {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{10};
        };
        // Frame interval is in 1/100 s.
        struct frame_interval {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{40};
        };
        // Time between 2 wrm movies (in seconds).
        struct break_interval {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{600};
        };
        // Number of png captures to keep.
        struct png_limit {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{5};
        };

        struct replay_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticString<1024>;
            type value{"/tmp/"};
        };

        struct hash_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticPath<1024>;
            type value{HASH_PATH};
        };
        struct record_tmp_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticPath<1024>;
            type value{RECORD_TMP_PATH};
        };
        struct record_path {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::StaticPath<1024>;
            type value{RECORD_PATH};
        };

        struct inactivity_pause {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = bool;
            type value{0};
        };
        struct inactivity_timeout {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = unsigned;
            type value{300};
        };

        // Disable keyboard log:
        //   1: disable keyboard log in syslog
        //   2: disable keyboard log in recorded sessions
        // AUTHID_DISABLE_KEYBOARD_LOG
        struct disable_keyboard_log {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 28; }
            using type = ::configs::KeyboardLogFlags;
            type value{};
        };

        // Disable clipboard log:
        //   1: disable clipboard log in syslog
        //   2: disable clipboard log in recorded sessions
        // AUTHID_DISABLE_CLIPBOARD_LOG
        struct disable_clipboard_log {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 29; }
            using type = ::configs::ClipboardLogFlags;
            type value{};
        };

        // Disable (redirected) file system log:
        //   1: disable (redirected) file system log in syslog
        //   2: disable (redirected) file system log in recorded sessions
        // AUTHID_DISABLE_FILE_SYSTEM_LOG
        struct disable_file_system_log {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 30; }
            using type = ::configs::FileSystemLogFlags;
            type value{};
        };

        // AUTHID_RT_DISPLAY
        struct rt_display {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::read;
            }
            static constexpr unsigned index() { return 31; }
            using type = unsigned;
            type value{0};
        };

        // The method by which the proxy RDP establishes criteria on which to chosse a color depth for native video capture:
        //   0: 24-bit
        //   1: 16-bit
        struct wrm_color_depth_selection_strategy {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Range<unsigned, 0, 1, 0>;
            type value{1};
        };
        // The compression method of native video capture:
        //   0: No compression
        //   1: GZip
        //   2: Snappy
        struct wrm_compression_algorithm {
            static constexpr ::configs::VariableProperties properties() {
                return ::configs::VariableProperties::none;
            }
            using type = ::configs::Range<unsigned, 0, 2, 0>;
            type value{1};
        };
    };

}

namespace cfg_section {
struct client
: cfg::client::keyboard_layout
, cfg::client::ignore_logon_password
, cfg::client::performance_flags_default
, cfg::client::performance_flags_force_present
, cfg::client::performance_flags_force_not_present
, cfg::client::tls_fallback_legacy
, cfg::client::tls_support
, cfg::client::bogus_neg_request
, cfg::client::bogus_user_id
, cfg::client::disable_tsk_switch_shortcuts
, cfg::client::rdp_compression
, cfg::client::max_color_depth
, cfg::client::persistent_disk_bitmap_cache
, cfg::client::cache_waiting_list
, cfg::client::persist_bitmap_cache_on_disk
, cfg::client::bitmap_compression
, cfg::client::fast_path
{ static constexpr bool is_section = true; };

struct context
: cfg::context::movie
, cfg::context::opt_bitrate
, cfg::context::opt_framerate
, cfg::context::opt_qscale
, cfg::context::opt_bpp
, cfg::context::opt_height
, cfg::context::opt_width
, cfg::context::auth_error_message
, cfg::context::selector
, cfg::context::selector_current_page
, cfg::context::selector_device_filter
, cfg::context::selector_group_filter
, cfg::context::selector_proto_filter
, cfg::context::selector_lines_per_page
, cfg::context::selector_number_of_pages
, cfg::context::target_password
, cfg::context::target_host
, cfg::context::target_service
, cfg::context::target_port
, cfg::context::target_protocol
, cfg::context::password
, cfg::context::reporting
, cfg::context::auth_channel_answer
, cfg::context::auth_channel_result
, cfg::context::auth_channel_target
, cfg::context::message
, cfg::context::accept_message
, cfg::context::display_message
, cfg::context::rejected
, cfg::context::authenticated
, cfg::context::keepalive
, cfg::context::session_id
, cfg::context::end_date_cnx
, cfg::context::end_time
, cfg::context::mode_console
, cfg::context::timezone
, cfg::context::real_target_device
, cfg::context::authentication_challenge
, cfg::context::ticket
, cfg::context::comment
, cfg::context::duration
, cfg::context::waitinforeturn
, cfg::context::showform
, cfg::context::formflag
, cfg::context::module
, cfg::context::forcemodule
, cfg::context::proxy_opt
{ static constexpr bool is_section = true; };

struct crypto
: cfg::crypto::key0
, cfg::crypto::key1
{ static constexpr bool is_section = true; };

struct debug
: cfg::debug::x224
, cfg::debug::mcs
, cfg::debug::sec
, cfg::debug::rdp
, cfg::debug::primary_orders
, cfg::debug::secondary_orders
, cfg::debug::bitmap
, cfg::debug::capture
, cfg::debug::auth
, cfg::debug::session
, cfg::debug::front
, cfg::debug::mod_rdp
, cfg::debug::mod_vnc
, cfg::debug::mod_int
, cfg::debug::mod_xup
, cfg::debug::widget
, cfg::debug::input
, cfg::debug::password
, cfg::debug::compression
, cfg::debug::cache
, cfg::debug::bitmap_update
, cfg::debug::performance
, cfg::debug::pass_dialog_box
, cfg::debug::config
{ static constexpr bool is_section = true; };

struct globals
: cfg::globals::capture_chunk
, cfg::globals::auth_user
, cfg::globals::host
, cfg::globals::target
, cfg::globals::target_device
, cfg::globals::target_user
, cfg::globals::target_application
, cfg::globals::target_application_account
, cfg::globals::target_application_password
, cfg::globals::bitmap_cache
, cfg::globals::glyph_cache
, cfg::globals::port
, cfg::globals::nomouse
, cfg::globals::notimestamp
, cfg::globals::encryptionLevel
, cfg::globals::authip
, cfg::globals::authport
, cfg::globals::session_timeout
, cfg::globals::keepalive_grace_delay
, cfg::globals::close_timeout
, cfg::globals::auth_channel
, cfg::globals::enable_file_encryption
, cfg::globals::listen_address
, cfg::globals::enable_ip_transparent
, cfg::globals::certificate_password
, cfg::globals::png_path
, cfg::globals::wrm_path
, cfg::globals::alternate_shell
, cfg::globals::shell_working_directory
, cfg::globals::movie
, cfg::globals::movie_path
, cfg::globals::enable_bitmap_update
, cfg::globals::enable_close_box
, cfg::globals::enable_osd
, cfg::globals::enable_osd_display_remote_target
, cfg::globals::enable_wab_agent
, cfg::globals::enable_wab_agent_loading_mask
, cfg::globals::wab_agent_launch_timeout
, cfg::globals::wab_agent_on_launch_failure
, cfg::globals::wab_agent_keepalive_timeout
, cfg::globals::wab_agent_alternate_shell
, cfg::globals::persistent_path
, cfg::globals::disable_proxy_opt
{ static constexpr bool is_section = true; };

struct internal_mod
: cfg::internal_mod::theme
{ static constexpr bool is_section = true; };

struct mod_rdp
: cfg::mod_rdp::rdp_compression
, cfg::mod_rdp::disconnect_on_logon_user_change
, cfg::mod_rdp::open_session_timeout
, cfg::mod_rdp::certificate_change_action
, cfg::mod_rdp::extra_orders
, cfg::mod_rdp::enable_nla
, cfg::mod_rdp::enable_kerberos
, cfg::mod_rdp::persistent_disk_bitmap_cache
, cfg::mod_rdp::cache_waiting_list
, cfg::mod_rdp::persist_bitmap_cache_on_disk
, cfg::mod_rdp::allow_channels
, cfg::mod_rdp::deny_channels
, cfg::mod_rdp::fast_path
, cfg::mod_rdp::server_redirection_support
, cfg::mod_rdp::redir_info
, cfg::mod_rdp::bogus_sc_net_size
, cfg::mod_rdp::client_device_announce_timeout
, cfg::mod_rdp::proxy_managed_drives
{ static constexpr bool is_section = true; };

struct mod_replay
: cfg::mod_replay::on_end_of_data
{ static constexpr bool is_section = true; };

struct mod_vnc
: cfg::mod_vnc::clipboard_up
, cfg::mod_vnc::clipboard_down
, cfg::mod_vnc::encodings
, cfg::mod_vnc::allow_authentification_retries
, cfg::mod_vnc::server_clipboard_encoding_type
, cfg::mod_vnc::bogus_clipboard_infinite_loop
{ static constexpr bool is_section = true; };

struct translation
: cfg::translation::language
{ static constexpr bool is_section = true; };

struct video
: cfg::video::capture_groupid
, cfg::video::capture_flags
, cfg::video::png_interval
, cfg::video::frame_interval
, cfg::video::break_interval
, cfg::video::png_limit
, cfg::video::replay_path
, cfg::video::hash_path
, cfg::video::record_tmp_path
, cfg::video::record_path
, cfg::video::inactivity_pause
, cfg::video::inactivity_timeout
, cfg::video::disable_keyboard_log
, cfg::video::disable_clipboard_log
, cfg::video::disable_file_system_log
, cfg::video::rt_display
, cfg::video::wrm_color_depth_selection_strategy
, cfg::video::wrm_compression_algorithm
{ static constexpr bool is_section = true; };

}

namespace configs {
struct VariablesConfiguration
: cfg_section::client
, cfg_section::context
, cfg_section::crypto
, cfg_section::debug
, cfg_section::globals
, cfg_section::internal_mod
, cfg_section::mod_rdp
, cfg_section::mod_replay
, cfg_section::mod_vnc
, cfg_section::translation
, cfg_section::video
, cfg::theme
, cfg::font
{
    explicit VariablesConfiguration(char const * default_font_name)
    : cfg::font{default_font_name}
    {}
};

using VariablesAclPack = Pack<
  cfg::globals::capture_chunk
, cfg::globals::auth_user
, cfg::globals::host
, cfg::globals::target
, cfg::globals::target_device
, cfg::globals::target_user
, cfg::globals::target_application
, cfg::globals::target_application_account
, cfg::globals::target_application_password
, cfg::globals::enable_file_encryption
, cfg::globals::alternate_shell
, cfg::globals::shell_working_directory
, cfg::globals::movie
, cfg::globals::movie_path
, cfg::globals::enable_wab_agent
, cfg::globals::enable_wab_agent_loading_mask
, cfg::globals::wab_agent_launch_timeout
, cfg::globals::wab_agent_on_launch_failure
, cfg::globals::wab_agent_keepalive_timeout
, cfg::client::keyboard_layout
, cfg::client::disable_tsk_switch_shortcuts
, cfg::mod_rdp::bogus_sc_net_size
, cfg::mod_rdp::client_device_announce_timeout
, cfg::mod_rdp::proxy_managed_drives
, cfg::mod_vnc::clipboard_up
, cfg::mod_vnc::clipboard_down
, cfg::mod_vnc::server_clipboard_encoding_type
, cfg::mod_vnc::bogus_clipboard_infinite_loop
, cfg::video::disable_keyboard_log
, cfg::video::disable_clipboard_log
, cfg::video::disable_file_system_log
, cfg::video::rt_display
, cfg::translation::language
, cfg::context::opt_bitrate
, cfg::context::opt_framerate
, cfg::context::opt_qscale
, cfg::context::opt_bpp
, cfg::context::opt_height
, cfg::context::opt_width
, cfg::context::auth_error_message
, cfg::context::selector
, cfg::context::selector_current_page
, cfg::context::selector_device_filter
, cfg::context::selector_group_filter
, cfg::context::selector_proto_filter
, cfg::context::selector_lines_per_page
, cfg::context::selector_number_of_pages
, cfg::context::target_password
, cfg::context::target_host
, cfg::context::target_service
, cfg::context::target_port
, cfg::context::target_protocol
, cfg::context::password
, cfg::context::reporting
, cfg::context::auth_channel_answer
, cfg::context::auth_channel_result
, cfg::context::auth_channel_target
, cfg::context::message
, cfg::context::accept_message
, cfg::context::display_message
, cfg::context::rejected
, cfg::context::authenticated
, cfg::context::keepalive
, cfg::context::session_id
, cfg::context::end_date_cnx
, cfg::context::end_time
, cfg::context::mode_console
, cfg::context::timezone
, cfg::context::real_target_device
, cfg::context::authentication_challenge
, cfg::context::ticket
, cfg::context::comment
, cfg::context::duration
, cfg::context::waitinforeturn
, cfg::context::showform
, cfg::context::formflag
, cfg::context::module
, cfg::context::forcemodule
, cfg::context::proxy_opt
>;
}
