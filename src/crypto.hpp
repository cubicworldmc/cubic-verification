#pragma once

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>

#define KEY_LEN 32
#define NONCE_LEN 12
#define TAG_LEN 16

namespace crypto {

class CryptoBase {
   public:
    inline std::vector<unsigned char> base64_decode(const std::string& key) {
        BIO* bio = BIO_new_mem_buf(key.data(), key.size());
        BIO* b64 = BIO_new(BIO_f_base64());

        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        bio = BIO_push(b64, bio);

        std::vector<unsigned char> output(key.size());

        size_t len = BIO_read(bio, output.data(), output.size());
        BIO_free_all(bio);

        if (len < 0) throw std::runtime_error("fcked to decode base64");

        output.resize(len);
        return output;
    }
};

class AES : public CryptoBase {
   public:
    AES(const std::string& key_file);

    std::string decrypt(const std::string& base64);

   private:
    std::array<unsigned char, 32> key;
};

class ChaCha20_Poly1305 : public CryptoBase {
   public:
    ChaCha20_Poly1305(const std::string& key_file);

    std::vector<unsigned char> encrypt(const std::string& plaintext);
    std::string decrypt(const std::vector<unsigned char>& message);

   private:
    std::array<unsigned char, 32> key;

   private:
    std::vector<unsigned char> generate_nonce();
};

}  // namespace crypto
