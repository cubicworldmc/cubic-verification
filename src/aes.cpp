#include <iostream>

#include "crypto.hpp"

using namespace crypto;

AES::AES(const std::string& key_file) {
    std::ifstream file(key_file);
    if (!file.is_open()) throw std::runtime_error("failed to open key file");

    std::string encoded((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    while (!encoded.empty() &&
           std::isspace(static_cast<unsigned char>(encoded.back()))) {
        encoded.pop_back();
    }

    size_t start = 0;
    while (start < encoded.size() &&
           std::isspace(static_cast<unsigned char>(encoded[start]))) {
        ++start;
    }

    encoded = encoded.substr(start);

    auto decoded = base64_decode(encoded);
    if (decoded.size() != 32)
        throw std::runtime_error("decoded key is not aes-256");

    std::copy(decoded.begin(), decoded.end(), key.begin());
}

std::string AES::decrypt(const std::string& base64) {
    std::vector<unsigned char> encrypted = base64_decode(base64);
    std::cout << encrypted.size() << std::endl;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("failed to create cipther ctx");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ecb(), nullptr, key.data(),
                           nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("decrypt init failed");
    }

    std::vector<unsigned char> out(encrypted.size() + 16);

    int out_len1 = 0;
    if (EVP_DecryptUpdate(ctx, out.data(), &out_len1, encrypted.data(),
                          encrypted.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("decrypt update failed");
    }

    int out_len2 = 0;
    if (EVP_DecryptFinal_ex(ctx, out.data() + out_len1, &out_len2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("invalid key");
    }

    EVP_CIPHER_CTX_free(ctx);

    out.resize(out_len1 + out_len2);

    return std::string(out.begin(), out.end());
}

std::vector<unsigned char> AES::base64_decode(const std::string& str) {
    BIO* bio = BIO_new_mem_buf(str.data(), str.size());
    BIO* b64 = BIO_new(BIO_f_base64());

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    std::vector<unsigned char> output(str.size());

    size_t len = BIO_read(bio, output.data(), output.size());
    BIO_free_all(bio);

    if (len < 0) throw std::runtime_error("fcked to decode base64");

    output.resize(len);
    return output;
}
