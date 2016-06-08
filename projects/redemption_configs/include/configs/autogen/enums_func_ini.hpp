#pragma once

#include "configs/io.hpp"
#include "configs/autogen/enums.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>


namespace configs {

template<> struct zstr_buffer_traits<CaptureFlags> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<CaptureFlags> & buf,
    cfg_s_type<CaptureFlags>,
    CaptureFlags x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(CaptureFlags & x, spec_type<CaptureFlags>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (5 - 1)) - 1));
}

template<> struct zstr_buffer_traits<Level> : zstr_buffer_traits<void> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<Level> & buf,
    cfg_s_type<Level>,
    Level x
) {
    (void)buf;    static constexpr array_view_const_char arr[]{
        cstr_array_view("low"),
        cstr_array_view("medium"),
        cstr_array_view("high"),
    };
    assert(static_cast<unsigned long>(x) < 3);
    return arr[static_cast<unsigned long>(x)];
}

inline parse_error parse(Level & x, spec_type<Level>, array_view_const_char value)
{
    return parse_enum_str(x, value, {
        {cstr_array_view("low"), Level::low},
        {cstr_array_view("medium"), Level::medium},
        {cstr_array_view("high"), Level::high},
    });
}

template<> struct zstr_buffer_traits<Language> : zstr_buffer_traits<void> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<Language> & buf,
    cfg_s_type<Language>,
    Language x
) {
    (void)buf;    static constexpr array_view_const_char arr[]{
        cstr_array_view("en"),
        cstr_array_view("fr"),
    };
    assert(static_cast<unsigned long>(x) < 2);
    return arr[static_cast<unsigned long>(x)];
}

inline parse_error parse(Language & x, spec_type<Language>, array_view_const_char value)
{
    return parse_enum_str(x, value, {
        {cstr_array_view("en"), Language::en},
        {cstr_array_view("fr"), Language::fr},
    });
}

template<> struct zstr_buffer_traits<ClipboardEncodingType> : zstr_buffer_traits<void> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<ClipboardEncodingType> & buf,
    cfg_s_type<ClipboardEncodingType>,
    ClipboardEncodingType x
) {
    (void)buf;    static constexpr array_view_const_char arr[]{
        cstr_array_view("utf8"),
        cstr_array_view("latin1"),
    };
    assert(static_cast<unsigned long>(x) < 2);
    return arr[static_cast<unsigned long>(x)];
}

inline parse_error parse(ClipboardEncodingType & x, spec_type<ClipboardEncodingType>, array_view_const_char value)
{
    return parse_enum_str(x, value, {
        {cstr_array_view("utf-8"), ClipboardEncodingType::utf8},
        {cstr_array_view("latin1"), ClipboardEncodingType::latin1},
    });
}

template<> struct zstr_buffer_traits<KeyboardLogFlags> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<KeyboardLogFlags> & buf,
    cfg_s_type<KeyboardLogFlags>,
    KeyboardLogFlags x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(KeyboardLogFlags & x, spec_type<KeyboardLogFlags>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<ClipboardLogFlags> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<ClipboardLogFlags> & buf,
    cfg_s_type<ClipboardLogFlags>,
    ClipboardLogFlags x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(ClipboardLogFlags & x, spec_type<ClipboardLogFlags>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<FileSystemLogFlags> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<FileSystemLogFlags> & buf,
    cfg_s_type<FileSystemLogFlags>,
    FileSystemLogFlags x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(FileSystemLogFlags & x, spec_type<FileSystemLogFlags>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<ServerNotification> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<ServerNotification> & buf,
    cfg_s_type<ServerNotification>,
    ServerNotification x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(ServerNotification & x, spec_type<ServerNotification>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (4 - 1)) - 1));
}

template<> struct zstr_buffer_traits<ServerCertCheck> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<ServerCertCheck> & buf,
    cfg_s_type<ServerCertCheck>,
    ServerCertCheck x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(ServerCertCheck & x, spec_type<ServerCertCheck>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (4 - 1)) - 1));
}

template<> struct zstr_buffer_traits<TraceType> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<TraceType> & buf,
    cfg_s_type<TraceType>,
    TraceType x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(TraceType & x, spec_type<TraceType>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<KeyboardInputMaskingLevel> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<KeyboardInputMaskingLevel> & buf,
    cfg_s_type<KeyboardInputMaskingLevel>,
    KeyboardInputMaskingLevel x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(KeyboardInputMaskingLevel & x, spec_type<KeyboardInputMaskingLevel>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (4 - 1)) - 1));
}

template<> struct zstr_buffer_traits<SessionProbeOnLaunchFailure> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<SessionProbeOnLaunchFailure> & buf,
    cfg_s_type<SessionProbeOnLaunchFailure>,
    SessionProbeOnLaunchFailure x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(SessionProbeOnLaunchFailure & x, spec_type<SessionProbeOnLaunchFailure>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<VncBogusClipboardInfiniteLoop> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<VncBogusClipboardInfiniteLoop> & buf,
    cfg_s_type<VncBogusClipboardInfiniteLoop>,
    VncBogusClipboardInfiniteLoop x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(VncBogusClipboardInfiniteLoop & x, spec_type<VncBogusClipboardInfiniteLoop>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<ColorDepthSelectionStrategy> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<ColorDepthSelectionStrategy> & buf,
    cfg_s_type<ColorDepthSelectionStrategy>,
    ColorDepthSelectionStrategy x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(ColorDepthSelectionStrategy & x, spec_type<ColorDepthSelectionStrategy>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (2 - 1)) - 1));
}

template<> struct zstr_buffer_traits<WrmCompressionAlgorithm> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<WrmCompressionAlgorithm> & buf,
    cfg_s_type<WrmCompressionAlgorithm>,
    WrmCompressionAlgorithm x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(WrmCompressionAlgorithm & x, spec_type<WrmCompressionAlgorithm>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (3 - 1)) - 1));
}

template<> struct zstr_buffer_traits<RdpCompression> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<RdpCompression> & buf,
    cfg_s_type<RdpCompression>,
    RdpCompression x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(RdpCompression & x, spec_type<RdpCompression>, array_view_const_char value)
{
    return parse_enum_u(x, value, static_cast<unsigned long>((1 << (5 - 1)) - 1));
}

template<> struct zstr_buffer_traits<ColorDepth> : zstr_buffer_traits<unsigned long> {};

inline array_view_const_char assign_zbuf_from_cfg(
    zstr_buffer_from<ColorDepth> & buf,
    cfg_s_type<ColorDepth>,
    ColorDepth x
) {
    int sz = snprintf(buf.get(), buf.size(), "%lu", static_cast<unsigned long>(x));
    return array_view_const_char(buf.get(), sz);
}

inline parse_error parse(ColorDepth & x, spec_type<ColorDepth>, array_view_const_char value)
{
    return parse_enum_list(x, value, {
        ColorDepth::depth8,
        ColorDepth::depth15,
        ColorDepth::depth16,
        ColorDepth::depth24,
    });
}

}
