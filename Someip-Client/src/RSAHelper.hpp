#ifndef RSA_ENCRYPTION_HPP
#define RSA_ENCRYPTION_HPP

#include <CommonAPI/CommonAPI.hpp>
#include <string>
// Encrypts a CommonAPI::ByteBuffer with an RSA public key
CommonAPI::ByteBuffer encryptByteBufferRSA(
    const CommonAPI::ByteBuffer& input, 
    const std::string& publicKeyPath = "");

CommonAPI::ByteBuffer decryptByteBufferRSA(
    const CommonAPI::ByteBuffer& input, 
    const std::string& privateKeyPath = "");

#endif // RSA_ENCRYPTION_HPP
