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
   Copyright (C) Wallix 2012
   Author(s): Christophe Grosjean

   Transport layer abstraction
*/

#pragma once

#ifdef REDEMPTION_SERVER_CERT_EXTERNAL_VALIDATION
#include <openssl/ssl.h>

enum class CertificateResult { wait, valid, invalid, unchecked, };
#endif

class ServerNotifier
{
public:
    virtual void server_access_allowed() = 0;
    virtual void server_cert_create() = 0;
    virtual void server_cert_success() = 0;
    virtual void server_cert_failure() = 0;
    virtual void server_cert_error(const char * str_error) = 0;

#ifdef REDEMPTION_SERVER_CERT_EXTERNAL_VALIDATION
    virtual CertificateResult server_cert_callback(const X509& certificate) = 0;
#endif

    virtual ~ServerNotifier() = default;
};

class NullServerNotifier : public ServerNotifier {
public:
    void server_access_allowed() override {}
    void server_cert_create() override {}
    void server_cert_success() override {}
    void server_cert_failure() override {}

#ifdef REDEMPTION_SERVER_CERT_EXTERNAL_VALIDATION
    CertificateResult server_cert_callback(const X509& certificate) override
    {
        (void)certificate;
        return CertificateResult::unchecked;
    }
#endif

    // TODO used array_view ?
    void server_cert_error(const char * str_error) override { (void)str_error; }
};

