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
 *   Copyright (C) Wallix 2010-2012
 *   Author(s): Christophe Grosjean, Jonathan Poelen
 */

#if !defined(__WRM_RECORDER_HPP__)
#define __WRM_RECORDER_HPP__

#include <errno.h>
#include "transport.hpp"
#include "RDP/RDPSerializer.hpp"
#include "meta_file.hpp"
#include "RDP/RDPDrawable.hpp"
#include "bitmap.hpp"
#include "stream.hpp"
#include "png.hpp"
#include "error.hpp"
#include "auto_buffer.hpp"
#include "cipher_transport.hpp"
#include "zlib.hpp"
#include "range_time_point.hpp"


template<std::size_t N>
struct HexadecimalOption
{
    unsigned char data[N];
    std::size_t size;

    HexadecimalOption()
    : size(0)
    {}

    /**
     * \param s value in hexadecimal base
     */
    bool parse(const std::string& s)
    {
        std::size_t n = s.size() / 2 + (s.size() & 1);
        if (n > N || !transform_string_hex_to_data(s, this->data))
            return false;
        this->size = n;
        while (n != N)
            this->data[n++] = 0;
        return true;
    }

private:
    static bool transform_string_hex_to_data(const std::string& s,
                                             unsigned char * pdata)
    {
        std::string::const_iterator first = s.begin();
        std::string::const_iterator last = s.end();
        char c;
        if (s.size() & 1)
            --last;
        for (; first != last; ++first, ++pdata)
        {
            if (0xf == (*pdata = transform_c_hex_to_c_data(*first)))
                return false;
            if (0xf == (c = transform_c_hex_to_c_data(*++first)))
                return false;
            *pdata = (*pdata << 4) + c;
        }
        if (s.size() & 1)
        {
            if (0xf == (*pdata = transform_c_hex_to_c_data(*first)))
                return false;
            *pdata <<= 4;
        }
        return true;
    }

    static unsigned char transform_c_hex_to_c_data(char c)
    {
        if ('a' <= c && c <= 'f')
            return c - 'a' + 0xa;
        if ('A' <= c && c <= 'F')
            return c - 'A' + 0xa;
        if ('0' > c || c > '9')
            return 0xf;
        return c - '0';
    }
};

typedef HexadecimalOption<EVP_MAX_KEY_LENGTH> HexadecimalKeyOption;
typedef HexadecimalOption<EVP_MAX_IV_LENGTH> HexadecimalIVOption;

struct InputType {
    enum enum_t {
        NOT_FOUND,
        META_TYPE,
        WRM_TYPE
    };

    typedef enum_t format_type;

    static InputType::enum_t string_to_type(const std::string& s)
    {
        if (s == "mwrm")
            return META_TYPE;
        if (s == "wrm")
            return WRM_TYPE;
        return NOT_FOUND;
    }
};


class WRMRecorder
{
    public:
    const EVP_CIPHER * cipher_mode;
    const unsigned char* cipher_key;
    const unsigned char* cipher_iv;
    ENGINE* cipher_impl;
    InFileTransport trans;
    InCipherTransport cipher_trans;

public:
    RDPUnserializer reader;

    Drawable * redrawable;

public:
    std::size_t idx_file;

    std::string path;
    std::size_t base_path_len;

public:
    bool only_filename;
    bool force_interpret_breakpoint;
    bool interpret_breakpoint_is_passed;

    bool _init_init_crypt(const EVP_CIPHER* in_crypt_mode,
                          const HexadecimalKeyOption & in_crypt_key, 
                          const HexadecimalIVOption & in_crypt_iv)
    {
        if (in_crypt_key.size)
        {
            LOG(LOG_INFO, "init_cipher");
            this->cipher_mode = (in_crypt_mode?in_crypt_mode
                                   :CipherMode::to_evp_cipher((CipherMode::enum_t)this->reader.data_meta.crypt_mode));
            if (!this->cipher_mode){
                return false;
            }
            if (!this->cipher_trans.start(this->cipher_mode, 
                                             in_crypt_key.data,
                                             in_crypt_iv.size ? in_crypt_iv.data : 0,
                                             0)){
                this->cipher_mode = 0;
                std::cerr << "Error in the initialization of the encryption" << std::endl;
                return false;
            }

            this->cipher_key = in_crypt_key.data;
            this->cipher_iv = in_crypt_iv.size ? in_crypt_iv.data : 0;
            this->cipher_impl = 0;
            this->reader.trans = &this->cipher_trans;
            this->trans.diff_size_is_error = false;
        }
        return true;
    }

    void wrm_recorder_init(
        InputType::enum_t itype,
        std::string & base_path,
        bool ignore_dir_for_meta_in_wrm,
        bool times_in_meta_are_false,
        bool force_interpret_breakpoint,
        range_time_point & range,
        const EVP_CIPHER* in_crypt_mode,
        HexadecimalKeyOption & in_crypt_key, 
        HexadecimalIVOption & in_crypt_iv,
        std::string & in_filename,
        uint idx_start)
    {
        this->set_basepath(base_path);
        this->only_filename = ignore_dir_for_meta_in_wrm;

        try
        {
            switch (itype) {
                case InputType::WRM_TYPE:
                {
                    if (in_crypt_key.size && !in_crypt_iv.size){
                        in_crypt_iv.size = sizeof(in_crypt_iv.data);
                        memcpy(in_crypt_iv.data, this->reader.data_meta.crypt_iv, sizeof(in_crypt_iv.data));
                    }
                    if (!this->_init_init_crypt(in_crypt_mode, in_crypt_key, in_crypt_iv)){
                        throw Error(ERR_WRM_INVALID_INIT_CRYPT);
                    }
                    const char * filename = in_filename.c_str();
                    LOG(LOG_INFO, "WRMRecorder opening file : %s", filename);
                    int fd = ::open(filename, O_RDONLY);
                    if (-1 == fd){
                        LOG(LOG_ERR, "Error opening wrm reader file : %s", strerror(errno));
                       throw Error(ERR_WRM_RECORDER_OPEN_FAILED);
                    }
                    this->trans.fd = fd;
                    if (!this->reader.selected_next_order())
                    {
                        std::cerr << in_filename << " is invalid wrm file" << std::endl;
                        throw Error(ERR_WRM_INVALID_FILE);
                    }
                    if (!this->reader.chunk_type == WRMChunk::META_FILE){
                        std::cerr << this->reader.data_meta << '\n'
                         << "Chunk META not found in " << filename << '\n' 
                         << ". Chunk is " << this->reader.chunk_type << std::endl;
                        throw Error(ERR_WRM_CHUNK_META_NOT_FOUND);
                    }
                        
                    char tmp_filename[1024];
                    size_t len = this->reader.stream.in_uint32_le();
                    this->reader.stream.in_copy_bytes((uint8_t*)tmp_filename, len);
                    tmp_filename[len] = 0;
                    --this->reader.remaining_order_count;
                    
                    const char * filename2 = tmp_filename;
                    if (this->only_filename)
                    {
                        const char * tmp = strrchr(filename2 + strlen(filename2), '/');
                        if (tmp){
                            filename2 = tmp+1;
                        }
                    }
                    if (this->base_path_len){
                        this->path.erase(this->base_path_len);
                        this->path += filename2;
                        filename2 = this->path.c_str();
                    }
                    
                    if (!this->reader.load_data(filename2)){
                        std::cerr << "invalid meta chunck in " << in_filename << std::endl;
                        throw Error(ERR_WRM_INVALID_META_CHUNK);
                    }
                    if (!this->reader.data_meta.files.empty())
                    {
                        if (idx_start >= this->reader.data_meta.files.size()){
                            std::cerr << "idx " << idx_start << " not found" << std::endl;
                            throw Error(ERR_WRM_IDX_NOT_FOUND);
                        }
                        if (!times_in_meta_are_false){
                            const std::vector<DataFile>& files = this->reader.data_meta.files;
                            if (files[0].start_sec){
                                const timeval tm = {files[0].start_sec, files[0].start_usec};
                                uint64_t time = 0;
                                for (uint idx = idx_start + 1; idx != files.size(); ++idx)
                                {
                                    const DataFile& data_file = files[idx];
                                    if (data_file.start_sec)
                                    {
                                        timeval tm2 = {data_file.start_sec, data_file.start_usec};
                                        uint64_t elapsed = difftimeval(tm2, tm) / 1000000;
                                        if (elapsed > range.left.time)
                                        {
                                            range.left.time -= time;
                                            break;
                                        }
                                        time = elapsed;
                                        idx_start = idx;
                                    }
                                }
                            }
                        }
                    }
                    else  if (idx_start >= this->reader.data_meta.files.size()){
                        std::cerr << "idx " << idx_start << " not found" << std::endl;
                        throw Error(ERR_WRM_IDX_NOT_FOUND);
                    }
                    if (idx_start != this->idx_file)
                    {
                        this->next_file(this->reader.data_meta.files[this->idx_file].wrm_filename.c_str());
                    }
                }
                break;
                case InputType::META_TYPE:
                {
                    if (!this->reader.load_data(in_filename.c_str()))
                    {
                        std::cerr << "open " << in_filename << ' ' << strerror(errno) << std::endl;
                        throw Error(ERR_WRM_FAILED_OPENING_META_FILENAME);
                    }
                    if (idx_start >= this->reader.data_meta.files.size()){
                        std::cerr << "idx " << idx_start << " not found" << std::endl;
                        throw Error(ERR_WRM_IDX_NOT_FOUND);
                    }
                    if (!times_in_meta_are_false){
                        const std::vector<DataFile>& files = this->reader.data_meta.files;
                        if (files[0].start_sec){
                            const timeval tm = {files[0].start_sec, files[0].start_usec};
                            uint64_t time = 0;
                            for (uint idx = idx_start + 1; idx != files.size(); ++idx)
                            {
                                const DataFile& data_file = files[idx];
                                if (data_file.start_sec)
                                {
                                    timeval tm2 = {data_file.start_sec, data_file.start_usec};
                                    uint64_t elapsed = difftimeval(tm2, tm) / 1000000;
                                    if (elapsed > range.left.time)
                                    {
                                        range.left.time -= time;
                                        break;
                                    }
                                    time = elapsed;
                                    idx_start = idx;
                                }
                            }
                        }
                    }
                    const char * filename = this->reader.data_meta.files[idx_start].wrm_filename.c_str();
                    
                    if (this->only_filename)
                    {
                        const char * tmp = strrchr(filename + strlen(filename), '/');
                        if (tmp){
                            filename = tmp+1;
                        }
                    }
                    if (this->base_path_len){
                        this->path.erase(this->base_path_len);
                        this->path += filename;
                        filename = this->path.c_str();
                    }
                    
                    if (in_crypt_key.size && !in_crypt_iv.size){
                        in_crypt_iv.size = sizeof(in_crypt_iv.data);
                        memcpy(in_crypt_iv.data, this->reader.data_meta.crypt_iv, sizeof(in_crypt_iv.data));
                    }
                    if (!this->_init_init_crypt(in_crypt_mode, in_crypt_key, in_crypt_iv)){
                        throw Error(ERR_CIPHER_START);
                    }

                    LOG(LOG_INFO, "WRMRecorder opening file : %s", filename);
                    int fd = ::open(filename, O_RDONLY);
                    if (-1 == fd){
                        LOG(LOG_ERR, "Error opening wrm reader file : %s", strerror(errno));
                       throw Error(ERR_WRM_RECORDER_OPEN_FAILED);
                    }
                    this->trans.fd = fd;
                    if (this->reader.selected_next_order() 
                    && this->reader.chunk_type == WRMChunk::META_FILE){
                        this->reader.stream.p = this->reader.stream.end;
                        this->reader.remaining_order_count = 0;
                    }
                    if (!this->reader.chunk_type == WRMChunk::META_FILE){
                        std::cerr << this->reader.data_meta << '\n'
                         << "Chunk META not found in " << filename << '\n' 
                         << ". Chunk is " << this->reader.chunk_type << std::endl;
                        throw Error(ERR_WRM_CHUNK_META_NOT_FOUND);
                    }
                }
                break;
                default:
                    std::cerr << "Input type not found" << std::endl;
                    throw Error(ERR_WRM);
            }
            this->idx_file = idx_start + 1;
            this->force_interpret_breakpoint = force_interpret_breakpoint;
        }
        catch (const Error& error)
        {
            std::cerr << "Error " << error.id << ": " << strerror(error.errnum) << std::endl;
            throw error;
        }
    }



public:
    WRMRecorder(const timeval & now,
                const EVP_CIPHER * mode = 0,
                const unsigned char* key = 0,
                const unsigned char* iv = 0,
                ENGINE* impl = 0)
    : cipher_mode(mode)
    , cipher_key(key)
    , cipher_iv(iv)
    , cipher_impl(impl)
    , trans(0, this->cipher_mode ? false : true)
    , cipher_trans(&trans)
    , reader(this->cipher_mode ? (Transport*)&this->cipher_trans : &this->trans,
             now,
             0, Rect())
    , redrawable(0)
    , idx_file(0)
    , path()
    , base_path_len(0)
    , only_filename(false)
    , force_interpret_breakpoint(false)
    , interpret_breakpoint_is_passed(false)
    {
        LOG(LOG_INFO, "WRMRecorder 3");
        if (this->cipher_mode 
        && !this->cipher_trans.start(this->cipher_mode, this->cipher_key, this->cipher_iv, this->cipher_impl))
        {
            LOG(LOG_ERR, "Error cipher start in NativeCapture");
            throw Error(ERR_CIPHER_START);
        }
    }

    WRMRecorder(const timeval & now,
                const std::string& filename, const std::string basepath = "",
                const EVP_CIPHER * mode = 0,
                const unsigned char* key = 0,
                const unsigned char* iv = 0,
                ENGINE* impl = 0)
    : cipher_mode(mode)
    , cipher_key(key)
    , cipher_iv(iv)
    , cipher_impl(impl)
    , trans(0, this->cipher_mode ? false : true)
    , cipher_trans(&trans)
    , reader(this->cipher_mode
             ? (Transport*)&this->cipher_trans : &this->trans,
             now,
             0, Rect())
    , redrawable(0)
    , idx_file(0)
    , path(basepath)
    , base_path_len(basepath.length())
    , only_filename(false)
    , force_interpret_breakpoint(false)
    , interpret_breakpoint_is_passed(false)
    {
        LOG(LOG_INFO, "WRMRecorder 1");
        if (this->base_path_len && this->path[this->base_path_len - 1] != '/'){
            this->path += '/';
            ++this->base_path_len;
        }
        
        if (this->cipher_mode 
        && !this->cipher_trans.start(this->cipher_mode, this->cipher_key, this->cipher_iv, this->cipher_impl))
        {
            LOG(LOG_ERR, "Error cipher start in NativeCapture");
            throw Error(ERR_CIPHER_START);
        }
        
        std::size_t pos = filename.find_last_not_of('.');
        if (pos != std::string::npos
        && pos < filename.size()
        && filename[pos] == 'm'){
            if (!this->reader.load_data(filename.c_str())){
                throw Error(ERR_RECORDER_FAILED_TO_OPEN_TARGET_FILE, errno);
            }
            if (this->reader.data_meta.files.empty()){
                throw Error(ERR_RECORDER_META_REFERENCE_WRM);
            }
            if (this->reader.data_meta.crypt_mode && !this->cipher_mode){
                throw Error(ERR_RECORDER_FILE_CRYPTED);
            }

            const char * filename = this->reader.data_meta.files[0].wrm_filename.c_str();
            if (this->only_filename)
            {
                const char * tmp = strrchr(filename + strlen(filename), '/');
                if (tmp){
                    filename = tmp+1;
                }
            }
            if (this->base_path_len){
                this->path.erase(this->base_path_len);
                this->path += filename;
                filename = this->path.c_str();
            }

            LOG(LOG_INFO, "WRMRecorder opening file : %s", filename);
            int fd = ::open(filename, O_RDONLY);
            if (-1 == fd){
                LOG(LOG_ERR, "Error opening wrm reader file : %s", strerror(errno));
               throw Error(ERR_WRM_RECORDER_OPEN_FAILED);
            }
            this->trans.fd = fd;

            ++this->idx_file;
            if (this->reader.selected_next_order() 
            && this->reader.chunk_type == WRMChunk::META_FILE){
                this->reader.stream.p = this->reader.stream.end;
                this->reader.remaining_order_count = 0;
            }
        }
        else {
        
            const char* tmp_filename = filename.c_str();
            
            LOG(LOG_INFO, "WRMRecorder opening file : %s", tmp_filename);
            int fd = ::open(tmp_filename, O_RDONLY);
            if (-1 == fd){
                LOG(LOG_ERR, "Error opening wrm reader file : %s", strerror(errno));
               throw Error(ERR_WRM_RECORDER_OPEN_FAILED);
            }
            this->trans.fd = fd;
        
            if (!this->reader.selected_next_order()){
                throw Error(ERR_RECORDER_META_REFERENCE_WRM, errno);
            }
            if (this->reader.chunk_type == WRMChunk::META_FILE) {
                char tmp_filename2[1024];
                size_t len = this->reader.stream.in_uint32_le();
                this->reader.stream.in_copy_bytes((uint8_t*)tmp_filename2, len);
                tmp_filename2[len] = 0;
                --this->reader.remaining_order_count;
                
                const char * filename2 = tmp_filename2;
                if (this->only_filename){
                    const char * tmp = strrchr(filename2 + strlen(filename2), '/');
                    if (tmp){
                        filename2 = tmp+1;
                    }
                }
                if (this->base_path_len){
                    this->path.erase(this->base_path_len);
                    this->path += filename2;
                    filename2 = this->path.c_str();
                }
                if (!this->reader.load_data(filename2)){
                    throw Error(ERR_RECORDER_META_REFERENCE_WRM, errno);
                }
            }
            if (this->reader.data_meta.crypt_mode 
            && !this->cipher_mode){
                throw Error(ERR_RECORDER_FILE_CRYPTED);
            }
        }
    }

    void load_png_context(Drawable& drawable)
    {
        if (this->idx_file > 1 
        && this->reader.data_meta.files.size() >= this->idx_file
        && !this->reader.data_meta.files[this->idx_file - 1].png_filename.empty())
        {
            this->redrawable = &drawable;
            this->load_context(this->reader.data_meta.files[this->idx_file - 1].png_filename.c_str());
            this->redrawable = 0;
        }
    }



    ~WRMRecorder()
    {
        ::close(this->trans.fd);
    }

public:
    void set_basepath(const std::string basepath)
    {
        this->base_path_len = basepath.length();
        this->path = basepath;
        if (this->base_path_len && this->path[this->base_path_len - 1] != '/')
        {
            this->path += '/';
            ++this->base_path_len;
        }
    }


public:
    void next_file(const char * filename)
    {
        ::close(this->trans.fd);
        if (this->cipher_mode){
            this->cipher_trans.stop();
        }
        this->trans.fd = -1;
        if (this->only_filename)
        {
            const char * tmp = strrchr(filename + strlen(filename), '/');
            if (tmp){
                filename = tmp+1;
            }
        }
        if (this->base_path_len){
            this->path.erase(this->base_path_len);
            this->path += filename;
            filename = this->path.c_str();
        }

        LOG(LOG_INFO, "WRMRecorder opening file : %s", filename);
        int fd = ::open(filename, O_RDONLY);
        if (-1 == fd){
            LOG(LOG_ERR, "Error opening wrm reader file : %s", strerror(errno));
           throw Error(ERR_WRM_RECORDER_OPEN_FAILED);
        }
        this->trans.fd = fd;

        this->trans.total_received = 0;
        this->trans.last_quantum_received = 0;
        this->trans.total_sent = 0;
        this->trans.last_quantum_sent = 0;
        this->trans.quantum_count = 0;
        if (this->cipher_mode){
            this->cipher_trans.reset();
        }
    }

    void load_context(const char * filename)
    {
        if (this->redrawable)
        {
            if (this->only_filename)
            {
                const char * tmp = strrchr(filename + strlen(filename), '/');
                if (tmp){
                    filename = tmp+1;
                }
            }
            if (this->base_path_len){
                this->path.erase(this->base_path_len);
                this->path += filename;
                filename = this->path.c_str();
            }
            std::FILE* fd = std::fopen(filename, "r");
            if (0 == fd)
            {
                LOG(LOG_ERR, "open context screen %s: %s", filename, strerror(errno));
                throw Error(ERR_RECORDER_FAILED_TO_OPEN_TARGET_FILE, errno);
            }

            if (this->cipher_mode){
                /*uint8_t crypt_buf[600*800*3];
                uint8_t uncrypt_buf[sizeof(crypt_buf) + EVP_MAX_BLOCK_LENGTH];
                CipherCryptData cipher_data(uncrypt_buf);
                CipherCrypt cipher_crypt(CipherCrypt::DecryptConstruct(), &cipher_data);
                cipher_crypt.start(this->cipher_mode,
                                   this->cipher_key,
                                   this->cipher_iv);
                std::size_t len = fread(crypt_buf, 1, sizeof(crypt_buf), fd);
                if (ferror(fd))
                {
                    LOG(LOG_ERR, "read context error : %s",
                        strerror(ferror(fd)));
                    throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                }
                cipher_crypt.update(crypt_buf, len);
                memcpy(this->redrawable->data, uncrypt_buf, cipher_data.size());*/
                z_stream zstrm;
                zstrm.zalloc = 0;
                zstrm.zfree = 0;
                zstrm.opaque = 0;
                int ret;
                if ((ret = inflateInit(&zstrm)) != Z_OK)
                {
                    LOG(LOG_ERR, "zlib: inflateInit: %d", ret);
                    throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                }
                ZRaiiInflateEnd infliate_end(zstrm);
                uint8_t crypt_buf[8192];
                uint8_t uncrypt_buf[sizeof(crypt_buf) + EVP_MAX_BLOCK_LENGTH];
                CipherCryptData cipher_data(uncrypt_buf);
                CipherCrypt cipher_crypt(CipherCrypt::DecryptConstruct(), &cipher_data);
                cipher_crypt.start(this->cipher_mode,
                                   this->cipher_key,
                                   this->cipher_iv);
                std::size_t len = 0;
                int flush;
                std::size_t pixlen = this->redrawable->height * this->redrawable->rowsize;
                do
                {
                    len = fread(crypt_buf, 1, sizeof(crypt_buf), fd);
                    if (ferror(fd))
                    {
                        LOG(LOG_ERR, "read context error : %s",
                            strerror(ferror(fd)));
                        throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                    }
                    cipher_data.reset();
                    cipher_crypt.update(crypt_buf, len);
                    if (feof(fd))
                    {
                        flush = Z_FINISH;
                        cipher_crypt.stop();
                    }
                    else
                    {
                        flush = Z_NO_FLUSH;
                    }
                    zstrm.avail_in = cipher_data.size();
                    zstrm.next_in = uncrypt_buf;
                    do
                    {
                        zstrm.avail_out = pixlen - zstrm.total_out;
                        zstrm.next_out = this->redrawable->data + zstrm.total_out;
                        ret = inflate(&zstrm, flush);
                        if (ret != Z_OK)
                        {
                            if (ret == Z_STREAM_END && flush == Z_FINISH)
                                break;
                            LOG(LOG_ERR, "zlib: inflate: %s", zError(ret));
                            throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                        }
                    } while(zstrm.avail_out == 0);
                    if (zstrm.avail_in != 0)
                    {
                        LOG(LOG_ERR, "read context error : %s",
                            strerror(ferror(fd)));
                        throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                    }
                } while (flush != Z_FINISH);
            }
            else
            {
                read_png24(fd,
                           this->redrawable->data,
                           this->redrawable->width,
                           this->redrawable->height,
                           this->redrawable->rowsize);
            }
            fclose(fd);
        }
    }


private:
    void recv_rect(Rect& rect)
    {
        rect.x = this->reader.stream.in_uint16_le();
        rect.y = this->reader.stream.in_uint16_le();
        rect.cx = this->reader.stream.in_uint16_le();
        rect.cy = this->reader.stream.in_uint16_le();
    }

    void recv_brush(RDPBrush& brush)
    {
        brush.org_x = this->reader.stream.in_uint8();
        brush.org_y = this->reader.stream.in_uint8();
        brush.style = this->reader.stream.in_uint8();
        brush.hatch = this->reader.stream.in_uint8();
        brush.extra[0] = this->reader.stream.in_uint8();
        brush.extra[1] = this->reader.stream.in_uint8();
        brush.extra[2] = this->reader.stream.in_uint8();
        brush.extra[3] = this->reader.stream.in_uint8();
        brush.extra[4] = this->reader.stream.in_uint8();
        brush.extra[5] = this->reader.stream.in_uint8();
        brush.extra[6] = this->reader.stream.in_uint8();
    }

    void recv_pen(RDPPen& pen)
    {
        pen.color = this->reader.stream.in_uint32_le();
        pen.style = this->reader.stream.in_uint8();
        pen.width = this->reader.stream.in_uint8();
    }

public:
    timeval get_start_time_order()
    {
        timeval time;
        time.tv_sec = this->reader.stream.in_uint64_be();
        time.tv_usec = this->reader.stream.in_uint64_be();
        return time;
    }

    void interpret_order()
    {
        switch (this->reader.chunk_type)
        {
            case WRMChunk::TIME_START:
            {
                this->reader.stream.p = this->reader.stream.end;
                this->reader.remaining_order_count = 0;
            }
            break;
            case WRMChunk::META_FILE:
            {
                this->reader.stream.p = this->reader.stream.end;
                this->reader.remaining_order_count = 0;
            }
            break;
            case WRMChunk::NEXT_FILE_ID:
            {
                this->idx_file = this->reader.stream.in_uint32_le();
                if (this->reader.data_meta.files.size() <= this->idx_file)
                {
                    LOG(LOG_ERR, "WRMRecorder : idx(%d) not found in meta", (int)this->idx_file);
                    throw Error(ERR_RECORDER_META_REFERENCE_WRM);
                }
                this->next_file(this->reader.data_meta.files[this->idx_file].wrm_filename.c_str());
                --this->reader.remaining_order_count;
                this->load_context(this->reader.data_meta.files[this->idx_file].png_filename.c_str());
            }
            break;
            case WRMChunk::BREAKPOINT:
            {
                if (!this->interpret_breakpoint_is_passed || this->force_interpret_breakpoint){
                    /*uint16_t width = */this->reader.stream.in_uint16_le();
                    /*uint16_t height = */this->reader.stream.in_uint16_le();
                    /*uint8_t bpp = */this->reader.stream.in_uint8();
                    this->reader.timer_cap.tv_sec = this->reader.stream.in_uint64_le();
                    this->reader.timer_cap.tv_usec = this->reader.stream.in_uint64_le();
                    --this->reader.remaining_order_count;

                    this->reader.selected_next_order();

                    this->reader.common.order = this->reader.stream.in_uint8();
                    this->recv_rect(this->reader.common.clip);
                    //this->reader.common.str(texttest, 10000);
                    //std::cout << "interpret_order: " << texttest << '\n';

                    this->reader.opaquerect.color = this->reader.stream.in_uint32_le();
                    this->recv_rect(this->reader.opaquerect.rect);
                    //std::cout << "interpret_order: ";
                    //this->reader.opaquerect.print(Rect(0,0,0,0));

                    this->reader.destblt.rop = this->reader.stream.in_uint8();
                    this->recv_rect(this->reader.destblt.rect);
                    //std::cout << "interpret_order: ";
                    //this->reader.destblt.print(Rect(0,0,0,0));

                    this->reader.patblt.rop = this->reader.stream.in_uint8();
                    this->reader.patblt.back_color = this->reader.stream.in_uint32_le();
                    this->reader.patblt.fore_color = this->reader.stream.in_uint32_le();
                    this->recv_brush(this->reader.patblt.brush);
                    this->recv_rect(this->reader.patblt.rect);
                    //std::cout << "interpret_order: ";
                    //this->reader.patblt.print(Rect(0,0,0,0));

                    this->reader.scrblt.rop = this->reader.stream.in_uint8();
                    this->reader.scrblt.srcx = this->reader.stream.in_uint16_le();
                    this->reader.scrblt.srcy = this->reader.stream.in_uint16_le();
                    this->recv_rect(this->reader.scrblt.rect);
                    //std::cout << "interpret_order: ";
                    //this->reader.scrblt.print(Rect(0,0,0,0));

                    this->reader.memblt.rop = this->reader.stream.in_uint8();
                    this->reader.memblt.srcx = this->reader.stream.in_uint16_le();
                    this->reader.memblt.srcy = this->reader.stream.in_uint16_le();
                    this->reader.memblt.cache_id = this->reader.stream.in_uint16_le();
                    this->reader.memblt.cache_idx = this->reader.stream.in_uint16_le();
                    this->recv_rect(this->reader.memblt.rect);
                    //std::cout << "interpret_order: ";
                    //this->reader.memblt.print(Rect(0,0,0,0));

                    this->reader.lineto.rop2 = this->reader.stream.in_uint8();
                    this->reader.lineto.startx = this->reader.stream.in_uint16_le();
                    this->reader.lineto.starty = this->reader.stream.in_uint16_le();
                    this->reader.lineto.endx = this->reader.stream.in_uint16_le();
                    this->reader.lineto.endy = this->reader.stream.in_uint16_le();
                    this->reader.lineto.back_mode = this->reader.stream.in_uint8();
                    this->reader.lineto.back_color = this->reader.stream.in_uint32_le();
                    this->recv_pen(this->reader.lineto.pen);
                    //std::cout << "interpret_order: ";
                    //this->reader.lineto.print(Rect(0,0,0,0));

                    this->reader.glyphindex.back_color = this->reader.stream.in_uint32_le();
                    this->reader.glyphindex.fore_color = this->reader.stream.in_uint32_le();
                    this->reader.glyphindex.f_op_redundant = this->reader.stream.in_uint16_le();
                    this->reader.glyphindex.fl_accel = this->reader.stream.in_uint16_le();
                    this->reader.glyphindex.glyph_x = this->reader.stream.in_uint16_le();
                    this->reader.glyphindex.glyph_y = this->reader.stream.in_uint16_le();
                    this->reader.glyphindex.ui_charinc = this->reader.stream.in_uint16_le();
                    this->reader.glyphindex.cache_id = this->reader.stream.in_uint8();
                    this->reader.glyphindex.data_len = this->reader.stream.in_uint8();
                    this->recv_rect(this->reader.glyphindex.bk);
                    this->recv_rect(this->reader.glyphindex.op);
                    this->recv_brush(this->reader.glyphindex.brush);
                    this->reader.glyphindex.data = (uint8_t*)malloc(this->reader.glyphindex.data_len);
                    this->reader.stream.in_copy_bytes(this->reader.glyphindex.data, this->reader.glyphindex.data_len);
                    //std::cout << "interpret_order: ";
                    //this->reader.glyphindex.print(Rect(0,0,0,0));

                    this->reader.order_count = this->reader.stream.in_uint16_le();
                    //std::cout << "\ninterpret_order: "  << this->reader.order_count << '\n';

                    this->reader.bmp_cache.small_entries = this->reader.stream.in_uint16_le();
                    this->reader.bmp_cache.small_size = this->reader.stream.in_uint16_le();
                    this->reader.bmp_cache.medium_entries = this->reader.stream.in_uint16_le();
                    this->reader.bmp_cache.medium_size = this->reader.stream.in_uint16_le();
                    this->reader.bmp_cache.big_entries = this->reader.stream.in_uint16_le();
                    this->reader.bmp_cache.big_size = this->reader.stream.in_uint16_le();
                    uint32_t stamp = this->reader.stream.in_uint32_le();

                    this->reader.bmp_cache.reset();
                    this->reader.bmp_cache.stamp = stamp;
                    this->reader.remaining_order_count = 0;

                    z_stream zstrm;
                    zstrm.zalloc = 0;
                    zstrm.zfree = 0;
                    zstrm.opaque = 0;
                    int ret;
                    const int Bpp = 3;
                    uint8_t * buffer = NULL;
                    while (1){
                        BStream stream(14);
                        this->reader.trans->recv(&stream.end, 14);
                        uint16_t idx = stream.in_uint16_le();
                        uint32_t stamp = stream.in_uint32_le();
                        uint16_t cx = stream.in_uint16_le();
                        uint16_t cy = stream.in_uint16_le();
                        uint32_t buffer_size = stream.in_uint32_le();
                        if (idx == 8192 * 3 + 1){
                            break;
                        }

                        BStream image_stream(buffer_size);
                        this->reader.trans->recv(&image_stream.end, buffer_size);

                        zstrm.avail_in = buffer_size;
                        zstrm.next_in = image_stream.data;

                        buffer = (uint8_t*)malloc(cx * cy * Bpp);
                        zstrm.avail_out = cx * cy * Bpp;
                        zstrm.next_out = buffer;

                        if ((ret = inflateInit(&zstrm)) != Z_OK)
                        {
                            LOG(LOG_ERR, "zlib: inflateInit: %d", ret);
                            throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                        }

                        ret = inflate(&zstrm, Z_FINISH);
                        inflateEnd(&zstrm);

                        if (ret != Z_STREAM_END)
                        {
                            LOG(LOG_ERR, "zlib: inflate: %d", ret);
                            throw Error(ERR_WRM_RECORDER_ZIP_UNCOMPRESS);
                        }

                        uint cid = idx / 8192;
                        uint cidx = idx % 8192;
                        this->reader.bmp_cache.stamps[cid][cidx] = stamp;
                        if (this->reader.bmp_cache.cache[cid][cidx] != 0){
                            LOG(LOG_ERR, "bmp_cache already used at %u:%u", cid, cidx);
                        }
                        this->reader.bmp_cache.cache[cid][cidx] = new Bitmap(24, 0, cx, cy, buffer, cx*cy);
                    }
                    this->interpret_breakpoint_is_passed = true;

                }
                else {
                    this->reader.stream.p = this->reader.stream.end;
                    this->reader.remaining_order_count = 0;

                    this->reader.selected_next_order();
                    this->reader.remaining_order_count = 0;
                    while (1){
                        this->reader.stream.init(14);
                        this->reader.trans->recv(&this->reader.stream.end, 14);
                        uint16_t idx = this->reader.stream.in_uint16_le();
                        this->reader.stream.p += 8;
                        uint32_t buffer_size = this->reader.stream.in_uint32_le();
                        if (idx == 8192 * 3 + 1){
                            break;
                        }
                        this->reader.stream.init(buffer_size);
                        this->reader.trans->recv(&this->reader.stream.end, buffer_size);
                    }
                }
            }
            break;
            default:
                this->reader.interpret_order();
                break;
        }
    }

    bool next_order()
    {
        if (this->reader.selected_next_order()){
            this->interpret_order();
            return true;
        }
        return false;
    }
};

#endif
