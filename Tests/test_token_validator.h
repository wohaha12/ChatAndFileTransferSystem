#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

/**
 * @brief Token验证器测试类
 * @details 测试Token的生成、验证、过期等功能
 */
class TokenValidatorTest : public QObject {
    Q_OBJECT
public:
    explicit TokenValidatorTest(QObject* parent = nullptr);
    ~TokenValidatorTest() override;

private slots:
    void testTokenGeneration();
    void testTokenValidation();
    void testTokenExpiration();
    void testTokenRenewal();
    void testTokenCleanup();
    void testInvalidToken();
    void testExpiredToken();
};

} // namespace Tests
