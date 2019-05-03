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
  Copyright (C) Wallix 2013
  Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/


#pragma once

#include "core/RDP/nla/sspi.hpp"
#include "core/RDP/nla/credssp.hpp"
#include "core/RDP/nla/ntlm/ntlm.hpp"
#include "core/RDP/tpdu_buffer.hpp"
#include "utils/hexdump.hpp"
#include "utils/translation.hpp"
#include "system/ssl_sha256.hpp"

#ifndef __EMSCRIPTEN__
#include "core/RDP/nla/kerberos/kerberos.hpp"
#endif

#include "transport/transport.hpp"

class rdpCredsspClient
{
private:
    int send_seq_num = 0;
    int recv_seq_num = 0;

    TSCredentials ts_credentials;
    TSRequest ts_request;
    static const size_t CLIENT_NONCE_LENGTH = 32;

    uint8_t SavedClientNonce[CLIENT_NONCE_LENGTH];
    Array PublicKey;
    Array ClientServerHash;
    Array ServerClientHash;

    Array ServicePrincipalName;
    SEC_WINNT_AUTH_IDENTITY identity;
    std::unique_ptr<SecurityFunctionTable> table
      = std::make_unique<UnimplementedSecurityFunctionTable>();
    bool restricted_admin_mode;
    SecInterface sec_interface;

    const char * target_host;
    Random & rand;
    TimeObj & timeobj;
    std::string& extra_message;
    Translation::language_t lang;
    const bool verbose;

    Transport & trans;
    std::function<Ntlm_SecurityFunctionTable::PasswordCallback(SEC_WINNT_AUTH_IDENTITY&)> set_password_cb;

    void init_public_key()
    {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::ntlm_init");

        // ============================================
        /* Get Public Key From TLS Layer and hostname */
        // ============================================

        auto const key = this->trans.get_public_key();
        this->PublicKey.init(key.size());
        this->PublicKey.copy(key);
    }

    void credssp_send()
    {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::send");
        StaticOutStream<65536> ts_request_emit;
        this->ts_request.emit(ts_request_emit);
        this->trans.send(ts_request_emit.get_bytes());
    }

    void SetHostnameFromUtf8(const uint8_t * pszTargetName) {
        size_t length = (pszTargetName && *pszTargetName) ? strlen(char_ptr_cast(pszTargetName)) : 0;
        this->ServicePrincipalName.init(length + 1);
        this->ServicePrincipalName.copy({pszTargetName, length});
        this->ServicePrincipalName.get_data()[length] = 0;
    }

    SEC_STATUS InitSecurityInterface(
        SecInterface secInter, const char* pszPrincipal,
        Array* pvLogonID, SEC_WINNT_AUTH_IDENTITY const* pAuthData)
    {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::InitSecurityInterface");

        this->table.reset();

        switch (secInter) {
            case NTLM_Interface:
                LOG(LOG_INFO, "Credssp: NTLM Authentication");
                this->table = std::make_unique<Ntlm_SecurityFunctionTable>(this->rand, this->timeobj, this->set_password_cb);
                break;
            case Kerberos_Interface:
                LOG(LOG_INFO, "Credssp: KERBEROS Authentication");
                #ifndef __EMSCRIPTEN__
                this->table = std::make_unique<Kerberos_SecurityFunctionTable>();
                #else
                this->table = std::make_unique<UnimplementedSecurityFunctionTable>();
                LOG(LOG_ERR, "Could not Initiate %u Security Interface!", this->sec_interface);
                assert(!"Unsupported Kerberos");
                #endif
                break;
        }

        return this->table->AcquireCredentialsHandle(pszPrincipal, pvLogonID, pAuthData);
    }

    static void ap_integer_increment_le(array_view_u8 number) {
        for (uint8_t& i : number) {
            if (i < 0xFF) {
                i++;
                break;
            }
            i = 0;
        }
    }

    static void ap_integer_decrement_le(array_view_u8 number) {
        for (uint8_t& i : number) {
            if (i > 0) {
                i--;
                break;
            }
            i = 0xFF;
        }
    }

    void credssp_generate_client_nonce() {
        LOG(LOG_DEBUG, "rdpCredsspClient::credssp generate client nonce");
        this->rand.random(this->SavedClientNonce, CLIENT_NONCE_LENGTH);
        this->credssp_set_client_nonce();
    }

    void credssp_get_client_nonce() {
        LOG(LOG_DEBUG, "rdpCredsspClient::credssp get client nonce");
        if (this->ts_request.clientNonce.size() == CLIENT_NONCE_LENGTH) {
            memcpy(this->SavedClientNonce, this->ts_request.clientNonce.get_data(), CLIENT_NONCE_LENGTH);
        }
    }
    void credssp_set_client_nonce() {
        LOG(LOG_DEBUG, "rdpCredsspClient::credssp set client nonce");
        if (this->ts_request.clientNonce.size() == 0) {
            this->ts_request.clientNonce.init(CLIENT_NONCE_LENGTH);
            memcpy(this->ts_request.clientNonce.get_data(), this->SavedClientNonce, CLIENT_NONCE_LENGTH);
        }
    }

    void credssp_generate_public_key_hash_client_to_server() {
        LOG(LOG_DEBUG, "rdpCredsspClient::generate credssp public key hash (client->server)");
        Array & SavedHash = this->ClientServerHash;
//        auto magic_hash = make_array_view("CredSSP Client-To-Server Binding Hash");
        SslSha256 sha256;
        uint8_t hash[SslSha256::DIGEST_LENGTH];
        sha256.update("CredSSP Client-To-Server Binding Hash\0"_av);
        sha256.update(make_array_view(this->SavedClientNonce));
        sha256.update(this->PublicKey.av());
        sha256.final(hash);
        SavedHash.init(sizeof(hash));
        memcpy(SavedHash.get_data(), hash, sizeof(hash));
    }

    void credssp_generate_public_key_hash_server_to_client() {
        LOG(LOG_DEBUG, "rdpCredsspClient::generate credssp public key hash (server->client)");
        Array & SavedHash = this->ServerClientHash;
        SslSha256 sha256;
        uint8_t hash[SslSha256::DIGEST_LENGTH];
        sha256.update("CredSSP Server-To-Client Binding Hash\0"_av);
        sha256.update(make_array_view(this->SavedClientNonce));
        sha256.update(this->PublicKey.av());
        sha256.final(hash);
        SavedHash.init(sizeof(hash));
        memcpy(SavedHash.get_data(), hash, sizeof(hash));
    }

    SEC_STATUS credssp_encrypt_public_key_echo() {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::encrypt_public_key_echo");
        uint32_t version = this->ts_request.use_version;

        array_view_u8 public_key = this->PublicKey.av();
        if (version >= 5) {
            this->credssp_generate_client_nonce();
            this->credssp_generate_public_key_hash_client_to_server();
            public_key = this->ClientServerHash.av();
        }

        return this->table->EncryptMessage(
            public_key, this->ts_request.pubKeyAuth, this->send_seq_num++);
    }

    SEC_STATUS credssp_decrypt_public_key_echo() {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::decrypt_public_key_echo");

        Array Buffer;

        SEC_STATUS const status = this->table->DecryptMessage(
            this->ts_request.pubKeyAuth.av(), Buffer, this->recv_seq_num++);

        if (status != SEC_E_OK) {
            if (this->ts_request.pubKeyAuth.size() == 0) {
                // report_error
                this->extra_message = " ";
                this->extra_message.append(TR(trkeys::err_login_password, this->lang));
                LOG(LOG_INFO, "Provided login/password is probably incorrect.");
            }
            LOG(LOG_ERR, "DecryptMessage failure: 0x%08X", status);
            return status;
        }

        const uint32_t version = this->ts_request.use_version;

        array_view_const_u8 public_key = this->PublicKey.av();
        if (version >= 5) {
            this->credssp_get_client_nonce();
            this->credssp_generate_public_key_hash_server_to_client();
            public_key = this->ServerClientHash.av();
        }

        array_view_u8 public_key2 = Buffer.av();

        if (public_key2.size() != public_key.size()) {
            LOG(LOG_ERR, "Decrypted Pub Key length or hash length does not match ! (%zu != %zu)", public_key2.size(), public_key.size());
            return SEC_E_MESSAGE_ALTERED; /* DO NOT SEND CREDENTIALS! */
        }

        if (version < 5) {
            // if we are client and protocol is 2,3,4
            // then get the public key minus one
            ap_integer_decrement_le(public_key2);
        }

        if (memcmp(public_key.data(), public_key2.data(), public_key.size()) != 0) {
            LOG(LOG_ERR, "Could not verify server's public key echo");

            LOG(LOG_ERR, "Expected (length = %zu):", public_key.size());
            hexdump_av_c(public_key);

            LOG(LOG_ERR, "Actual (length = %zu):", public_key.size());
            hexdump_av_c(public_key2);

            return SEC_E_MESSAGE_ALTERED; /* DO NOT SEND CREDENTIALS! */
        }

        return SEC_E_OK;
    }

    void credssp_buffer_free() {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::buffer_free");
        this->ts_request.negoTokens.init(0);
        this->ts_request.pubKeyAuth.init(0);
        this->ts_request.authInfo.init(0);
        this->ts_request.clientNonce.init(0);
        this->ts_request.error_code = 0;
    }

    enum class Res : bool { Err, Ok };

    Res sm_credssp_client_authenticate_start()
    {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::client_authenticate");

        this->init_public_key();

        SEC_STATUS status = this->InitSecurityInterface(this->sec_interface, this->target_host,
                                                        &this->ServicePrincipalName,
                                                        &this->identity);

        if (status == SEC_E_NO_CREDENTIALS && this->sec_interface == Kerberos_Interface) {
            LOG(LOG_INFO, "Credssp: No Kerberos Credentials, fallback to NTLM");
            status = this->InitSecurityInterface(NTLM_Interface, this->target_host,
                                                 &this->ServicePrincipalName, &this->identity);
        }

        if (status != SEC_E_OK) {
            LOG(LOG_ERR, "InitSecurityInterface status: 0x%08X", status);
            return Res::Err;
        }

        this->client_auth_data.input_buffer.init(0);

        return Res::Ok;
    }

    struct ClientAuthenticateData
    {
        enum : uint8_t { Start, Loop, Final } state = Start;
        Array input_buffer;
    };
    ClientAuthenticateData client_auth_data;

    Res sm_credssp_client_authenticate_send()
    {
        /*
         * from tspkg.dll: 0x00000132
         * ISC_REQ_MUTUAL_AUTH
         * ISC_REQ_CONFIDENTIALITY
         * ISC_REQ_USE_SESSION_KEY
         * ISC_REQ_ALLOCATE_MEMORY
         */
        //unsigned long const fContextReq
        //  = ISC_REQ_MUTUAL_AUTH | ISC_REQ_CONFIDENTIALITY | ISC_REQ_USE_SESSION_KEY;

        SEC_STATUS status = this->table->InitializeSecurityContext(
            bytes_view(this->ServicePrincipalName.av()).as_chars(),
            this->client_auth_data.input_buffer.av(),
            /*output*/this->ts_request.negoTokens);
        if ((status != SEC_I_COMPLETE_AND_CONTINUE) &&
            (status != SEC_I_COMPLETE_NEEDED) &&
            (status != SEC_E_OK) &&
            (status != SEC_I_CONTINUE_NEEDED)) {
            LOG(LOG_ERR, "Initialize Security Context Error !");
            return Res::Err;
        }

        this->client_auth_data.input_buffer.init(0);

        SEC_STATUS encrypted = SEC_E_INVALID_TOKEN;
        if ((status == SEC_I_COMPLETE_AND_CONTINUE) ||
            (status == SEC_I_COMPLETE_NEEDED) ||
            (status == SEC_E_OK)) {
            // have_pub_key_auth = true;
            encrypted = this->credssp_encrypt_public_key_echo();
            if (status == SEC_I_COMPLETE_NEEDED) {
                status = SEC_E_OK;
            }
            else if (status == SEC_I_COMPLETE_AND_CONTINUE) {
                status = SEC_I_CONTINUE_NEEDED;
            }
        }

        /* send authentication token to server */

        if (this->ts_request.negoTokens.size() > 0) {
            // #ifdef WITH_DEBUG_CREDSSP
            //             LOG(LOG_ERR, "Sending Authentication Token");
            //             hexdump_c(this->ts_request.negoTokens.pvBuffer, this->ts_request.negoTokens.cbBuffer);
            // #endif
            LOG_IF(this->verbose, LOG_INFO, "rdpCredssp - Client Authentication : Sending Authentication Token");

            this->credssp_send();
            this->credssp_buffer_free();
        }
        else if (encrypted == SEC_E_OK) {
            this->credssp_send();
            this->credssp_buffer_free();
        }

        if (status != SEC_I_CONTINUE_NEEDED) {
            LOG_IF(this->verbose, LOG_INFO, "rdpCredssp Token loop: CONTINUE_NEEDED");

            this->client_auth_data.state = ClientAuthenticateData::Final;
            return Res::Ok;
        }
        /* receive server response and place in input buffer */

        return Res::Ok;
    }

    Res sm_credssp_client_authenticate_recv(InStream & in_stream)
    {
        this->ts_request.recv(in_stream);

        // #ifdef WITH_DEBUG_CREDSSP
        // LOG(LOG_ERR, "Receiving Authentication Token (%d)", (int) this->ts_request.negoTokens.cbBuffer);
        // hexdump_c(this->ts_request.negoTokens.pvBuffer, this->ts_request.negoTokens.cbBuffer);
        // #endif
        LOG_IF(this->verbose, LOG_INFO, "rdpCredssp - Client Authentication : Receiving Authentication Token");
        this->client_auth_data.input_buffer.copy(this->ts_request.negoTokens);

        return Res::Ok;
    }

    Res sm_credssp_client_authenticate_stop(InStream & in_stream)
    {
        /* Encrypted Public Key +1 */
        LOG_IF(this->verbose, LOG_INFO, "rdpCredssp - Client Authentication : Receiving Encrypted PubKey + 1");

        this->ts_request.recv(in_stream);

        /* Verify Server Public Key Echo */

        SEC_STATUS status = this->credssp_decrypt_public_key_echo();
        this->credssp_buffer_free();

        if (status != SEC_E_OK) {
            LOG(LOG_ERR, "Could not verify public key echo!");
            this->credssp_buffer_free();
            return Res::Err;
        }

        /* Send encrypted credentials */

        status = this->credssp_encrypt_ts_credentials();

        if (status != SEC_E_OK) {
            LOG(LOG_ERR, "credssp_encrypt_ts_credentials status: 0x%08X", status);
            return Res::Err;
        }

        LOG_IF(this->verbose, LOG_INFO, "rdpCredssp - Client Authentication : Sending Credentials");

        this->credssp_send();

        /* Free resources */
        this->credssp_buffer_free();


        return Res::Ok;
    }

    void credssp_encode_ts_credentials() {
        if (this->restricted_admin_mode) {
            LOG(LOG_INFO, "Restricted Admin Mode");
            this->ts_credentials.set_credentials(nullptr, 0,
                                                 nullptr, 0,
                                                 nullptr, 0);
        }
        else {
            this->ts_credentials.set_credentials(this->identity.Domain.get_data(),
                                                 this->identity.Domain.size(),
                                                 this->identity.User.get_data(),
                                                 this->identity.User.size(),
                                                 this->identity.Password.get_data(),
                                                 this->identity.Password.size());
        }
    }

    SEC_STATUS credssp_encrypt_ts_credentials() {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::encrypt_ts_credentials");
        this->credssp_encode_ts_credentials();

        StaticOutStream<65536> ts_credentials_send;
        this->ts_credentials.emit(ts_credentials_send);

        return this->table->EncryptMessage(
            {ts_credentials_send.get_data(), ts_credentials_send.get_offset()},
            this->ts_request.authInfo, this->send_seq_num++);
    }

    void set_credentials(uint8_t const* user, uint8_t const* domain,
                         uint8_t const* pass, uint8_t const* hostname) {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::set_credentials");
        this->identity.SetUserFromUtf8(user);
        this->identity.SetDomainFromUtf8(domain);
        this->identity.SetPasswordFromUtf8(pass);
        this->SetHostnameFromUtf8(hostname);
        // hexdump_c(user, strlen((char*)user));
        // hexdump_c(domain, strlen((char*)domain));
        // hexdump_c(pass, strlen((char*)pass));
        // hexdump_c(hostname, strlen((char*)hostname));
        this->identity.SetKrbAuthIdentity(user, pass);
    }

public:
    rdpCredsspClient(OutTransport transport,
               uint8_t * user,
               uint8_t * domain,
               uint8_t * pass,
               uint8_t * hostname,
               const char * target_host,
               const bool krb,
               const bool restricted_admin_mode,
               Random & rand,
               TimeObj & timeobj,
               std::string& extra_message,
               Translation::language_t lang,
               const bool verbose = false)
        : ts_request(6) // Credssp Version 6 Supported
        , SavedClientNonce()
        , restricted_admin_mode(restricted_admin_mode)
        , sec_interface(krb ? Kerberos_Interface : NTLM_Interface)
        , target_host(target_host)
        , rand(rand)
        , timeobj(timeobj)
        , extra_message(extra_message)
        , lang(lang)
        , verbose(verbose)
        , trans(transport.get_transport())
    {
        LOG_IF(this->verbose, LOG_INFO, "rdpCredsspClient::Initialization");
        this->set_credentials(user, domain, pass, hostname);
    }

    bool credssp_client_authenticate_init()
    {
        this->client_auth_data.state = ClientAuthenticateData::Start;
        if (Res::Err == this->sm_credssp_client_authenticate_start()) {
            return false;
        }

        this->client_auth_data.state = ClientAuthenticateData::Loop;
        return Res::Err != this->sm_credssp_client_authenticate_send();
    }


    enum class State { Err, Cont, Finish, };

    State credssp_client_authenticate_next(InStream & in_stream)
    {
        switch (this->client_auth_data.state)
        {
            case ClientAuthenticateData::Start:
                return State::Err;
            case ClientAuthenticateData::Loop:
                if (Res::Err == this->sm_credssp_client_authenticate_recv(in_stream)
                 || Res::Err == this->sm_credssp_client_authenticate_send()) {
                    return State::Err;
                }
                return State::Cont;
            case ClientAuthenticateData::Final:
                if (Res::Err == this->sm_credssp_client_authenticate_stop(in_stream)) {
                    return State::Err;
                }
                this->client_auth_data.state = ClientAuthenticateData::Start;
                return State::Finish;
        }

        return State::Err;
    }
};
