#include "test_token_validator.h"
#include "../../StorageServer/Core/TokenValidator.h"
#include <QtTest>
#include <thread>
#include <chrono>

namespace Tests {

/**
 * @brief 构造函数
 */
TokenValidatorTest::TokenValidatorTest(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 */
TokenValidatorTest::~TokenValidatorTest()
{
}

/**
 * @brief 测试Token生成
 * @details 验证能够正确生成Token
 */
void TokenValidatorTest::testTokenGeneration()
{
    StorageServer::Core::TokenValidator validator;
    
    QString token = validator.generateUploadToken(12345, "d41d8cd98f00b204e9800998ecf8427e", 1024 * 1024);
    
    QVERIFY(!token.isEmpty());
    QCOMPARE(token.length(), 64);
}

/**
 * @brief 测试Token验证
 * @details 验证能够正确验证Token
 */
void TokenValidatorTest::testTokenValidation()
{
    StorageServer::Core::TokenValidator validator;
    
    QString token = validator.generateUploadToken(12345, "d41d8cd98f00b204e9800998ecf8427e", 1024 * 1024);
    
    uint64_t userId;
    QString fileHash;
    uint64_t fileSize;
    
    QVERIFY(validator.validateUploadToken(token, userId, fileHash, fileSize));
    QCOMPARE(userId, static_cast<uint64_t>(12345));
    QCOMPARE(fileHash, QString("d41d8cd98f00b204e9800998ecf8427e"));
    QCOMPARE(fileSize, static_cast<uint64_t>(1024 * 1024));
}

/**
 * @brief 测试Token过期
 * @details 验证Token过期后无法验证
 */
void TokenValidatorTest::testTokenExpiration()
{
    StorageServer::Core::TokenValidator validator;
    validator.setTokenExpireTime(1);
    
    QString token = validator.generateUploadToken(12345, "d41d8cd98f00b204e9800998ecf8427e", 1024 * 1024);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    uint64_t userId;
    QString fileHash;
    uint64_t fileSize;
    
    QVERIFY(!validator.validateUploadToken(token, userId, fileHash, fileSize));
}

/**
 * @brief 测试Token续期
 * @details 验证能够正确续期Token
 */
void TokenValidatorTest::testTokenRenewal()
{
    StorageServer::Core::TokenValidator validator;
    validator.setTokenExpireTime(10);
    validator.setMaxRenewCount(5);
    
    QString token = validator.generateUploadToken(12345, "d41d8cd98f00b204e9800998ecf8427e", 1024 * 1024);
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    QVERIFY(validator.renewToken(token));
    
    uint64_t userId;
    QString fileHash;
    uint64_t fileSize;
    
    QVERIFY(validator.validateUploadToken(token, userId, fileHash, fileSize));
}

/**
 * @brief 测试Token清理
 * @details 验证能够正确清理过期Token
 */
void TokenValidatorTest::testTokenCleanup()
{
    StorageServer::Core::TokenValidator validator;
    validator.setTokenExpireTime(1);
    
    QString token1 = validator.generateUploadToken(12345, "d41d8cd98f00b204e9800998ecf8427e", 1024 * 1024);
    QString token2 = validator.generateUploadToken(12346, "d41d8cd98f00b204e9800998ecf8427f", 1024 * 1024);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    validator.cleanupExpiredTokens();
    
    uint64_t userId;
    QString fileHash;
    uint64_t fileSize;
    
    QVERIFY(!validator.validateUploadToken(token1, userId, fileHash, fileSize));
    QVERIFY(!validator.validateUploadToken(token2, userId, fileHash, fileSize));
}

/**
 * @brief 测试无效Token
 * @details 验证无效Token无法通过验证
 */
void TokenValidatorTest::testInvalidToken()
{
    StorageServer::Core::TokenValidator validator;
    
    QString invalidToken = "invalid_token_12345";
    
    uint64_t userId;
    QString fileHash;
    uint64_t fileSize;
    
    QVERIFY(!validator.validateUploadToken(invalidToken, userId, fileHash, fileSize));
}

/**
 * @brief 测试过期Token
 * @details 验证过期Token无法通过验证
 */
void TokenValidatorTest::testExpiredToken()
{
    StorageServer::Core::TokenValidator validator;
    validator.setTokenExpireTime(1);
    
    QString token = validator.generateUploadToken(12345, "d41d8cd98f00b204e9800998ecf8427e", 1024 * 1024);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    uint64_t userId;
    QString fileHash;
    uint64_t fileSize;
    
    QVERIFY(!validator.validateUploadToken(token, userId, fileHash, fileSize));
}

} // namespace Tests
