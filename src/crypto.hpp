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

class AES {
   public:
    AES(const std::string& key_file);

    std::string decrypt(const std::string& base64);

   private:
    std::array<unsigned char, 32> key;

   private:
    std::vector<unsigned char> base64_decode(const std::string& str);
};

class ChaCha20_Poly1305 {
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
