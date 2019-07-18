/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2016
    Author(s): Christophe Grosjean, Raphael Zhou
*/

#include "test_only/test_framework/redemption_unit_tests.hpp"

#include "core/FSCC/FileInformation.hpp"


#define FSCC_TEST(type, suffix, raw_data) RED_AUTO_TEST_CASE(Test##type##suffix) { \
    array_view_const_char in_data = raw_data;                                      \
    InStream in_stream(in_data);                                                   \
    StaticOutStream<1200> out_stream;                                              \
    fscc::type type;                                                               \
    type.receive(in_stream);                                                       \
    RED_CHECK_EQUAL(in_data.size(), type.size());                                  \
    type.emit(out_stream);                                                         \
    RED_CHECK_MEM(out_stream.get_bytes(), in_data);                                \
}

FSCC_TEST(FileBothDirectoryInformation, 1,
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xcf\x70\x3a\x49\xd1\x01" // ...........p:I..
    "\x00\x49\x4e\x80\x3a\x49\xd1\x01\x00\x00\xcf\x70\x3a\x49\xd1\x01" // .IN.:I.....p:I..
    "\x00\x00\xcf\x70\x3a\x49\xd1\x01\x00\xa2\x1f\x00\x00\x00\x00\x00" // ...p:I..........
    "\x00\xb0\x1f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00" // ................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" // ................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x42\x00\x49" // .............B.I
    "\x00\x4e\x00"                                                     // .N...
    ""_av)

FSCC_TEST(FileBothDirectoryInformation, 2,
    "\x00\x00\x00\x00\x00\x00\x00\x00\x50\xea\xef\xf9\xe4\x20\xce\x01" // ........P.... ..
    "\xa0\x27\xf5\x05\x2b\x4a\xd1\x01\x00\x7f\xa7\x9f\x53\x21\xce\x01" // .'..+J......S!..
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" // ................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x16\x00\x00\x00\x32\x00\x00\x00" // ............2...
    "\x00\x00\x00\x00\x10\x53\x00\x59\x00\x53\x00\x54\x00\x45\x00\x4d" // .....S.Y.S.T.E.M
    "\x00\x7e\x00\x31\x00\x00\x00\x00\x00\x00\x00\x00\x00\x53\x00\x79" // .~.1.........S.y
    "\x00\x73\x00\x74\x00\x65\x00\x6d\x00\x20\x00\x56\x00\x6f\x00\x6c" // .s.t.e.m. .V.o.l
    "\x00\x75\x00\x6d\x00\x65\x00\x20\x00\x49\x00\x6e\x00\x66\x00\x6f" // .u.m.e. .I.n.f.o
    "\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x69\x00\x6f\x00\x6e\x00"     //.r.m.a.t.i.o.n.
    ""_av)

FSCC_TEST(FileFullDirectoryInformation, 1,
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x91\x70\x1b\xee\x4e\xd1\x01" // ..........p..N..
    "\x00\xb7\x64\x66\xa2\x4e\xd1\x01\x00\x91\x70\x1b\xee\x4e\xd1\x01" // ..df.N....p..N..
    "\x00\x18\x04\x1f\xee\x4e\xd1\x01\x00\xa4\x1f\x00\x00\x00\x00\x00" // .....N..........
    "\x00\xc0\x1f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00" // ................
    "\x00\x00\x00\x00\x42\x00\x49\x00\x4e\x00"                         // ....B.I.N.
    ""_av)

FSCC_TEST(FileFsAttributeInformation, 1,
    "\x07\x00\x00\x00\xff\x00\x00\x00\x0a\x00\x00\x00\x46\x00\x41\x00" // ............F.A.
    "\x54\x00\x33\x00\x32\x00"                                         // T.3.2.
    ""_av);

FSCC_TEST(FileFsAttributeInformation, 2,
    "\xff\x00\x07\x00\xff\x00\x00\x00\x08\x00\x00\x00\x4e\x00\x54\x00" // ............N.T.
    "\x46\x00\x53\x00"                                     // F.S.
    ""_av);

FSCC_TEST(FileFsVolumeInformation, 1,
    "\x80\xc3\x0f\x2f\x72\x4c\xd1\x01\xc6\x8c\x17\x36\x0e\x00\x00\x00" // .../rL.....6....
    "\x00\x46\x00\x52\x00\x45\x00\x45\x00\x52\x00\x44\x00\x50\x00"     // .F.R.E.E.R.D.P.
    ""_av);

FSCC_TEST(FileFsVolumeInformation, 2,
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00\x00"
    ""_av);

FSCC_TEST(FileObjectBuffer_Type1, 1,
    "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //@...............
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00"                                                 //....
    ""_av);

FSCC_TEST(FileObjectBuffer_Type2, 1,
    "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //@...............
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //................
    "\x00\x00\x00\x00"                                                 //....
    ""_av);

// FSCC_TEST(ReparseGUIDDataBuffer, 1, ""_av)

FSCC_TEST(FileAttributeTagInformation, 1,
    "\x20\x00\x00\x00\x00\x00\x00\x00"  //.... .......
    ""_av);

FSCC_TEST(FileBasicInformation, 1,
    "\x00\x02\x58\xcb\x31\x61\xd2\x01\x00\x76\x3e\x43" //$.....X.1a...v>C
    "\x86\x99\xd2\x01\x00\x02\x58\xcb\x31\x61\xd2\x01\x00\x20\xcf\x53" //......X.1a... .S
    "\xaa\x70\xd2\x01\x10\x00\x00\x00"                                 //.p......
    ""_av);

FSCC_TEST(FileDirectoryInformation, 1,
    "\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x6e\x7f\x7b" //k............n.{
    "\x88\x78\xd2\x01\x80\x36\x91\x6a\xe5\x98\xd2\x01\x80\x6e\x7f\x7b" //.x...6.j.....n.{
    "\x88\x78\xd2\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //.x..............
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00" //................
    "\x0e\x00\x00\x00\x73\x00\x68\x00\x61\x00\x72\x00\x65\x00\x2d\x00\x32\x00"     //.s.h.a.r.e.-.2.
    ""_av);

FSCC_TEST(FileDispositionInformation, 1,
    "\x10" //.
    ""_av);

FSCC_TEST(FileNamesInformation, 1,
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x0e\x00\x00\x00\x73\x00\x68\x00\x61\x00\x72\x00\x65\x00\x2d\x00\x32\x00"     //.s.h.a.r.e.-.2.
    ""_av);

FSCC_TEST(FileStandardInformation, 1,
    "\x00\x10\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00" //................
    "\x00\x00\x00\x00\x05\x00\x00\x00\x01\x01"         //..........
    ""_av);

FSCC_TEST(FileFsSizeInformation, 1,
    "\x00\x80\xad\x63\x50\x00\x00\x00"
    "\x00\x80\xad\x62\x50\x00\x00\x00\x08\x00\x00\x00\x00\x10\x00\x00"
    ""_av);
