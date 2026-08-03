#pragma once

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include <array>
#include <fstream>
#include <string>
#include <vector>

class Crypto {
   public:
    Crypto(const std::string& key_file, bool is_hex);

    std::string decrypt(const std::string& base64);

   private:
    std::array<unsigned char, 32> key;

   private:
    std::vector<unsigned char> base64_decode(const std::string& str);
};
