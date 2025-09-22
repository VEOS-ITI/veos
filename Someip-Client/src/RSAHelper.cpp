#include <CommonAPI/CommonAPI.hpp>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <memory>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>

// Helper function to get key path from environment or use default
std::string getKeyPath(const std::string& envVar, const std::string& defaultPath) {
    const char* envPath = std::getenv(envVar.c_str());
    return envPath ? envPath : defaultPath;
}

// Encrypt ByteBuffer with RSA public key
CommonAPI::ByteBuffer encryptByteBufferRSA(const CommonAPI::ByteBuffer& input, const std::string& publicKeyPath) {
    std::string actualPublicKeyPath = publicKeyPath.empty() ? 
        getKeyPath("RSA_PUBLIC_KEY_PATH", "public.pem") : publicKeyPath;
    
    std::cout << "[DEBUG] Using public key path: " << actualPublicKeyPath << std::endl;

    // 1. Load public key
    FILE* pubKeyFile = fopen(actualPublicKeyPath.c_str(), "rb");
    if (!pubKeyFile) {
        throw std::runtime_error("Cannot open public key file: " + actualPublicKeyPath);
    }
    RSA* rsaPubKey = PEM_read_RSA_PUBKEY(pubKeyFile, nullptr, nullptr, nullptr);
    fclose(pubKeyFile);
    if (!rsaPubKey) {
        throw std::runtime_error("Failed to load RSA public key from: " + actualPublicKeyPath);
    }

    // 2. Encrypt
    std::vector<unsigned char> encrypted(RSA_size(rsaPubKey));
    int result = RSA_public_encrypt(
        static_cast<int>(input.size()),
        input.data(),
        encrypted.data(),
        rsaPubKey,
        RSA_PKCS1_OAEP_PADDING
    );

    RSA_free(rsaPubKey);

    if (result == -1) {
        throw std::runtime_error("RSA encryption failed: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
    }

    // 3. Convert back to ByteBuffer
    return CommonAPI::ByteBuffer(encrypted.begin(), encrypted.begin() + result);
}

// Decrypt ByteBuffer with RSA private key
CommonAPI::ByteBuffer decryptByteBufferRSA(const CommonAPI::ByteBuffer& input, const std::string& privateKeyPath) {
    std::string actualPrivateKeyPath = privateKeyPath.empty() ? 
        getKeyPath("RSA_PRIVATE_KEY_PATH", "private.pem") : privateKeyPath;

    std::cout << "[DEBUG] Using private key path: " << actualPrivateKeyPath << std::endl;

    // 1. Load private key
    FILE* privKeyFile = fopen(actualPrivateKeyPath.c_str(), "rb");
    if (!privKeyFile) {
        throw std::runtime_error("Cannot open private key file: " + actualPrivateKeyPath);
    }
    RSA* rsaPrivKey = PEM_read_RSAPrivateKey(privKeyFile, nullptr, nullptr, nullptr);
    fclose(privKeyFile);
    if (!rsaPrivKey) {
        throw std::runtime_error("Failed to load RSA private key from: " + actualPrivateKeyPath);
    }

    // 2. Decrypt
    std::vector<unsigned char> decrypted(RSA_size(rsaPrivKey));
    int result = RSA_private_decrypt(
        static_cast<int>(input.size()),
        input.data(),
        decrypted.data(),
        rsaPrivKey,
        RSA_PKCS1_OAEP_PADDING
    );

    RSA_free(rsaPrivKey);

    if (result == -1) {
        throw std::runtime_error("RSA decryption failed: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
    }

    // 3. Convert back to ByteBuffer
    return CommonAPI::ByteBuffer(decrypted.begin(), decrypted.begin() + result);
}