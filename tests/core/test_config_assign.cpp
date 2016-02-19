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
*   Copyright (C) Wallix 2010-2016
*   Author(s): Jonathan Poelen
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestStrings
#include <boost/test/auto_unit_test.hpp>

#define LOGPRINT

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH
#include "config.hpp"

BOOST_AUTO_TEST_CASE(TestIniAssign)
{
    Inifile ini;

    char const * cpath = "/dsds/dsds";
    char const * cs = "blah blah";
    char const * cslist = "blah, blah";
    char const * cip = "0.0.0.0";
    std::string s(cs);
    std::string slist(cslist);
    std::string spath(cpath);
    std::string sip(cip);
    char key[32]{};

    ini.set<cfg::client::bitmap_compression>(true);
    ini.set<cfg::client::bogus_neg_request>(true);
    ini.set<cfg::client::bogus_user_id>(true);
    ini.set<cfg::client::cache_waiting_list>(true);
    ini.set<cfg::client::disable_tsk_switch_shortcuts>(1);
    ini.set<cfg::client::fast_path>(true);
    ini.set<cfg::client::ignore_logon_password>(true);
    ini.set_acl<cfg::client::keyboard_layout>(1);
    ini.set<cfg::client::keyboard_layout_proposals>(cslist);
    ini.set<cfg::client::keyboard_layout_proposals>(slist);
    ini.set<cfg::client::max_color_depth>(configs::ColorDepth::depth16);
    ini.set<cfg::client::performance_flags_default>(1);
    ini.set<cfg::client::performance_flags_force_not_present>(1);
    ini.set<cfg::client::performance_flags_force_present>(1);
    ini.set<cfg::client::persist_bitmap_cache_on_disk>(true);
    ini.set<cfg::client::rdp_compression>(1);
    ini.set<cfg::client::tls_fallback_legacy>(true);
    ini.set<cfg::client::tls_support>(true);

    ini.set_acl<cfg::context::accept_message>(cs);
    ini.set_acl<cfg::context::accept_message>(s);
    ini.set<cfg::context::auth_channel_answer>(cs);
    ini.set<cfg::context::auth_channel_answer>(s);
    ini.set<cfg::context::auth_channel_result>(cs);
    ini.set<cfg::context::auth_channel_result>(s);
    ini.set_acl<cfg::context::auth_channel_target>(cs);
    ini.set_acl<cfg::context::auth_channel_target>(s);
    ini.set<cfg::context::auth_error_message>(cs);
    ini.set<cfg::context::auth_error_message>(s);
    ini.set_acl<cfg::context::authenticated>(true);
    ini.set<cfg::context::authentication_challenge>(true);
    ini.set_acl<cfg::context::comment>(cs);
    ini.set_acl<cfg::context::comment>(s);
    ini.set<cfg::crypto::key0>(key);
    ini.set<cfg::crypto::key0>("12\x00#:\x55", 6u);
    ini.set<cfg::crypto::key0>(cstr_array_view("12\x00#:\x55"));
    ini.set_acl<cfg::context::display_message>(cs);
    ini.set_acl<cfg::context::display_message>(s);
    ini.set_acl<cfg::context::duration>(cs);
    ini.set_acl<cfg::context::duration>(s);
    ini.set<cfg::context::end_date_cnx>(1);
    ini.set<cfg::context::end_time>(cs);
    ini.set<cfg::context::end_time>(s);
    ini.set<cfg::context::forcemodule>(true);
    ini.set_acl<cfg::context::formflag>(1);
    ini.set<cfg::context::keepalive>(true);
    ini.set<cfg::context::message>(cs);
    ini.set<cfg::context::message>(s);
    ini.set<cfg::context::mode_console>(cs);
    ini.set<cfg::context::mode_console>(s);
    ini.set_acl<cfg::context::module>(1);
    ini.set<cfg::context::movie>(cpath);
    ini.set<cfg::context::movie>(spath);
    ini.set<cfg::context::opt_bitrate>(1);
    ini.set_acl<cfg::context::opt_bpp>(1);
    ini.set<cfg::context::opt_framerate>(1);
    ini.set_acl<cfg::context::opt_height>(1);
    ini.set<cfg::context::opt_message>(cs);
    ini.set<cfg::context::opt_message>(s);
    ini.set_acl<cfg::context::opt_width>(1);
    ini.set<cfg::context::outbound_connection_blocking_rules>(cs);
    ini.set<cfg::context::outbound_connection_blocking_rules>(s);
    ini.set_acl<cfg::context::password>(cs);
    ini.set_acl<cfg::context::password>(s);
    ini.set<cfg::context::pattern_kill>(cs);
    ini.set<cfg::context::pattern_kill>(cs);
    ini.set<cfg::context::pattern_kill>(s);
    ini.set<cfg::context::pattern_notify>(cs);
    ini.set<cfg::context::pattern_notify>(s);
    ini.set<cfg::context::proxy_opt>(cs);
    ini.set<cfg::context::proxy_opt>(s);
    ini.set_acl<cfg::context::real_target_device>(cs);
    ini.set_acl<cfg::context::real_target_device>(s);
    ini.set_acl<cfg::context::rejected>(cs);
    ini.set_acl<cfg::context::rejected>(s);
    ini.set_acl<cfg::context::reporting>(cs);
    ini.set_acl<cfg::context::reporting>(s);
    ini.set<cfg::context::selector>(true);
    ini.set_acl<cfg::context::selector_current_page>(1);
    ini.set<cfg::context::selector_device_filter>(cs);
    ini.set<cfg::context::selector_device_filter>(s);
    ini.set<cfg::context::selector_group_filter>(cs);
    ini.set<cfg::context::selector_group_filter>(s);
    ini.set_acl<cfg::context::selector_lines_per_page>(1);
    ini.set<cfg::context::selector_number_of_pages>(1);
    ini.set<cfg::context::selector_proto_filter>(cs);
    ini.set<cfg::context::selector_proto_filter>(s);
    ini.set<cfg::context::session_id>(cs);
    ini.set<cfg::context::session_id>(s);
    ini.set<cfg::context::showform>(true);
    ini.set_acl<cfg::context::target_host>(cs);
    ini.set_acl<cfg::context::target_host>(s);
    ini.set_acl<cfg::context::target_password>(cs);
    ini.set_acl<cfg::context::target_password>(s);
    ini.set_acl<cfg::context::target_port>(1);
    ini.set<cfg::context::target_protocol>(cs);
    ini.set<cfg::context::target_protocol>(s);
    ini.set<cfg::context::target_service>(cs);
    ini.set<cfg::context::target_service>(s);
    ini.set_acl<cfg::context::ticket>(cs);
    ini.set_acl<cfg::context::ticket>(s);
    ini.set<cfg::context::timezone>(1);
    ini.set_acl<cfg::context::waitinforeturn>(cs);
    ini.set_acl<cfg::context::waitinforeturn>(s);

    ini.set<cfg::crypto::key0>("12\x00#:\x55", 6u);
    ini.set<cfg::crypto::key0>(cstr_array_view("12\x00#:\x55"));
    ini.set<cfg::crypto::key0>(key);
    // is_same<key0:type, key1:type>
    [](cfg::crypto::key0::type *){}(static_cast<cfg::crypto::key1::type *>(nullptr));

    ini.set_acl<cfg::globals::globals::auth_user>(cs);
    ini.set_acl<cfg::globals::globals::auth_user>(s);
    ini.set<cfg::globals::globals::authfile>(cs);
    ini.set<cfg::globals::globals::authfile>(s);
    ini.set<cfg::globals::globals::bitmap_cache>(true);
    ini.set<cfg::globals::globals::capture_chunk>(true);
    ini.set<cfg::globals::globals::certificate_password>(cs);
    ini.set<cfg::globals::globals::certificate_password>(s);
    ini.set<cfg::globals::globals::close_timeout>(1);
    ini.set<cfg::globals::globals::device_id>(cs);
    ini.set<cfg::globals::globals::device_id>(s);
    ini.set<cfg::globals::globals::disable_proxy_opt>(true);
    ini.set<cfg::globals::globals::enable_bitmap_update>(true);
    ini.set<cfg::globals::globals::enable_close_box>(true);
    ini.set<cfg::globals::globals::enable_ip_transparent>(true);
    ini.set<cfg::globals::globals::enable_osd>(true);
    ini.set<cfg::globals::globals::enable_osd_display_remote_target>(true);
    ini.set<cfg::globals::globals::encryptionLevel>(configs::Level::high);
    ini.set<cfg::globals::globals::glyph_cache>(true);
    ini.set_acl<cfg::globals::globals::host>(cs);
    ini.set_acl<cfg::globals::globals::host>(s);
    ini.set<cfg::globals::globals::keepalive_grace_delay>(1);
    ini.set<cfg::globals::globals::listen_address>(cip);
    ini.set<cfg::globals::globals::listen_address>(sip);
    ini.set<cfg::globals::globals::movie>(true);
    ini.set<cfg::globals::globals::movie_path>(cpath);
    ini.set<cfg::globals::globals::movie_path>(spath);
    ini.set<cfg::globals::globals::nomouse>(true);
    ini.set<cfg::globals::globals::notimestamp>(true);
    ini.set<cfg::globals::globals::persistent_path>(cpath);
    ini.set<cfg::globals::globals::persistent_path>(spath);
    ini.set<cfg::globals::globals::png_path>(cpath);
    ini.set<cfg::globals::globals::png_path>(spath);
    ini.set<cfg::globals::globals::port>(1);
    ini.set<cfg::globals::globals::session_timeout>(1);
    ini.set_acl<cfg::globals::globals::target>(cs);
    ini.set_acl<cfg::globals::globals::target>(s);
    ini.set<cfg::globals::globals::target_application>(cs);
    ini.set<cfg::globals::globals::target_application>(s);
    ini.set<cfg::globals::globals::target_application_account>(cs);
    ini.set<cfg::globals::globals::target_application_account>(s);
    ini.set<cfg::globals::globals::target_application_password>(cs);
    ini.set<cfg::globals::globals::target_application_password>(s);
    ini.set<cfg::globals::globals::target_device>(cs);
    ini.set<cfg::globals::globals::target_device>(s);
    ini.set_acl<cfg::globals::globals::target_user>(cs);
    ini.set_acl<cfg::globals::globals::target_user>(s);
    ini.set<cfg::globals::globals::trace_type>(configs::TraceType::localfile);
    ini.set<cfg::globals::globals::wrm_path>(cpath);
    ini.set<cfg::globals::globals::wrm_path>(spath);

    ini.set<cfg::internal_mod::theme>(cpath);
    ini.set<cfg::internal_mod::theme>(spath);

    ini.set<cfg::mod_rdp::allow_channels>(cslist);
    ini.set<cfg::mod_rdp::allow_channels>(slist);
    ini.set<cfg::mod_rdp::alternate_shell>(cs);
    ini.set<cfg::mod_rdp::alternate_shell>(s);
    ini.set<cfg::mod_rdp::auth_channel>(cs);
    ini.set<cfg::mod_rdp::auth_channel>(s);
    ini.set<cfg::mod_rdp::bogus_sc_net_size>(true);
    ini.set<cfg::mod_rdp::cache_waiting_list>(true);
    ini.set<cfg::mod_rdp::deny_channels>(cslist);
    ini.set<cfg::mod_rdp::deny_channels>(slist);
    ini.set<cfg::mod_rdp::disconnect_on_logon_user_change>(true);
    ini.set<cfg::mod_rdp::enable_kerberos>(true);
    ini.set<cfg::mod_rdp::enable_nla>(true);
    ini.set<cfg::mod_rdp::enable_session_probe>(true);
    ini.set<cfg::mod_rdp::enable_session_probe_launch_mask>(true);
    ini.set<cfg::mod_rdp::extra_orders>(cslist);
    ini.set<cfg::mod_rdp::extra_orders>(slist);
    ini.set<cfg::mod_rdp::fast_path>(true);
    ini.set<cfg::mod_rdp::ignore_auth_channel>(true);
    ini.set<cfg::mod_rdp::open_session_timeout>(1);
    ini.set<cfg::mod_rdp::persist_bitmap_cache_on_disk>(true);
    ini.set<cfg::mod_rdp::persistent_disk_bitmap_cache>(true);
    ini.set<cfg::mod_rdp::proxy_managed_drives>(cslist);
    ini.set<cfg::mod_rdp::proxy_managed_drives>(slist);
    ini.set<cfg::mod_rdp::rdp_compression>(1);
    ini.get_ref<cfg::mod_rdp::redir_info>().dont_store_username = true;
    ini.set<cfg::mod_rdp::server_access_allowed_message>(configs::ServerNotification::admin);
    ini.set<cfg::mod_rdp::server_cert_check>(configs::ServerCertCheck::always_succeed);
    ini.set<cfg::mod_rdp::server_cert_create_message>(configs::ServerNotification::admin);
    ini.set<cfg::mod_rdp::server_cert_error_message>(configs::ServerNotification::admin);
    ini.set<cfg::mod_rdp::server_cert_failure_message>(configs::ServerNotification::admin);
    ini.set<cfg::mod_rdp::server_cert_store>(true);
    ini.set<cfg::mod_rdp::server_cert_success_message>(configs::ServerNotification::admin);
    ini.set<cfg::mod_rdp::server_redirection_support>(true);
    ini.set<cfg::mod_rdp::session_probe_alternate_shell>(s);
    ini.set<cfg::mod_rdp::session_probe_customize_executable_name>(true);
    ini.set<cfg::mod_rdp::session_probe_end_disconnected_session>(true);
    ini.set<cfg::mod_rdp::session_probe_keepalive_timeout>(1);
    ini.set<cfg::mod_rdp::session_probe_launch_timeout>(1);
    ini.set<cfg::mod_rdp::session_probe_on_launch_failure>(configs::SessionProbeOnLaunchFailure::ignore_and_continue);
    ini.set<cfg::mod_rdp::shell_working_directory>(cs);
    ini.set<cfg::mod_rdp::shell_working_directory>(s);

    ini.set<cfg::mod_replay::on_end_of_data>(true);

    ini.set<cfg::mod_vnc::allow_authentification_retries>(true);
    ini.set<cfg::mod_vnc::bogus_clipboard_infinite_loop>(1);
    ini.set<cfg::mod_vnc::clipboard_down>(true);
    ini.set<cfg::mod_vnc::clipboard_up>(true);
    ini.set<cfg::mod_vnc::encodings>(cslist);
    ini.set<cfg::mod_vnc::encodings>(slist);
    ini.set<cfg::mod_vnc::server_clipboard_encoding_type>(configs::ClipboardEncodingType::latin1);

    ini.set<cfg::session_log::enable_session_log>(true);
    ini.set<cfg::session_log::keyboard_input_masking_level>(configs::KeyboardInputMaskingLevel::unmasked);

    ini.set<cfg::translation::language>(configs::Language::en);
    ini.set<cfg::translation::password_en>(cs);
    ini.set<cfg::translation::password_en>(s);
    ini.set<cfg::translation::password_fr>(cs);
    ini.set<cfg::translation::password_fr>(s);

    ini.set<cfg::video::break_interval>(1);
    ini.set<cfg::video::capture_flags>(configs::CaptureFlags::wrm | configs::CaptureFlags::png);
    ini.set<cfg::video::capture_groupid>(1);
    ini.set<cfg::video::disable_clipboard_log>(configs::ClipboardLogFlags::syslog);
    ini.set<cfg::video::disable_file_system_log>(configs::FileSystemLogFlags::syslog);
    ini.set<cfg::video::disable_keyboard_log>(configs::KeyboardLogFlags::syslog);
    ini.set<cfg::video::frame_interval>(1);
    ini.set<cfg::video::hash_path>(cpath);
    ini.set<cfg::video::hash_path>(spath);
    ini.set<cfg::video::inactivity_pause>(1);
    ini.set<cfg::video::inactivity_timeout>(true);
    ini.set<cfg::video::png_interval>(1);
    ini.set<cfg::video::png_limit>(1);
    ini.set<cfg::video::record_path>(cpath);
    ini.set<cfg::video::record_path>(spath);
    ini.set<cfg::video::record_tmp_path>(cpath);
    ini.set<cfg::video::record_tmp_path>(spath);
    ini.set<cfg::video::replay_path>(cpath);
    ini.set<cfg::video::replay_path>(spath);
    ini.set<cfg::video::rt_display>(1);
    ini.set<cfg::video::wrm_color_depth_selection_strategy>(1);
    ini.set<cfg::video::wrm_compression_algorithm>(1);
}
