#include <iostream>

#include "crypto.hpp"

using namespace crypto;

ChaCha20_Poly1305::ChaCha20_Poly1305(const std::string& key_file) {
    std::ifstream file(key_file);
    if (!file.is_open()) throw std::runtime_error("failed to open key file");

    std::string encoded;
    std::getline(file, encoded);

    std::vector<unsigned char> decoded = base64_decode(encoded);

    if (decoded.size() != KEY_LEN)
        throw std::runtime_error("key must be 32 bytesa");

    std::copy(decoded.begin(), decoded.end(), key.begin());
}

std::vector<unsigned char> ChaCha20_Poly1305::encrypt(
    const std::string& plaintext) {
    std::vector<unsigned char> nonce = generate_nonce();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("failed to create cipher ctx");

    std::vector<unsigned char> ciphertext(plaintext.size());
    std::vector<unsigned char> tag(TAG_LEN);

    if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, nullptr,
                           nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to init cipher");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, NONCE_LEN, nullptr) !=
        1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to set nonce (sex offender) length");
    }

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce.data()) !=
        1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to init encrypt");
    }

    int len = 0;
    int ciphertext_len = 0;

    if (EVP_EncryptUpdate(
            ctx, ciphertext.data(), &len,
            reinterpret_cast<const unsigned char*>(plaintext.data()),
            plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("encryption failed");
    }

    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + ciphertext_len, &len) !=
        1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("encryption final failed");
    }

    ciphertext_len += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, TAG_LEN, tag.data()) !=
        1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to get auth tag");
    }

    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(ciphertext_len);

    std::vector<unsigned char> result;
    result.reserve(NONCE_LEN + ciphertext.size() + TAG_LEN);
    result.insert(result.end(), nonce.begin(), nonce.end());
    result.insert(result.end(), ciphertext.begin(), ciphertext.end());
    result.insert(result.end(), tag.begin(), tag.end());

    return result;
}

std::string ChaCha20_Poly1305::decrypt(
    const std::vector<unsigned char>& message) {
    if (message.size() < NONCE_LEN + TAG_LEN)
        throw std::runtime_error("encrypted message is too smalll");

    const unsigned char* nonce = message.data();
    const unsigned char* tag = message.data() + message.size() - TAG_LEN;
    const unsigned char* ciphertext = message.data() + NONCE_LEN;
    size_t               ciphertext_len = message.size() - NONCE_LEN - TAG_LEN;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("failed to create cipher ctx");

    if (EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, nullptr,
                           nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to init cipher");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, NONCE_LEN, nullptr) !=
        1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to set nonce length");
    }

    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to sey key and nonce");
    }

    std::vector<unsigned char> plaintext(ciphertext_len);

    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext,
                          ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("decryption failed");
    }

    plaintext_len = len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, TAG_LEN,
                            const_cast<unsigned char*>(tag)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("failed to set auth tag");
    }

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + plaintext_len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("auth failed");
    }

    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(plaintext_len);

    return std::string(plaintext.begin(), plaintext.end());
}

std::vector<unsigned char> ChaCha20_Poly1305::generate_nonce() {
    std::vector<unsigned char> nonce(NONCE_LEN);
    if (!RAND_bytes(nonce.data(), NONCE_LEN))
        throw std::runtime_error("failed to generate nonce");

    return nonce;
}
