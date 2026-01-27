#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

/**
 * @brief 协议测试类
 * @details 测试通信协议的编解码、校验等功能
 */
class ProtocolTest : public QObject {
    Q_OBJECT
public:
    explicit ProtocolTest(QObject* parent = nullptr);
    ~ProtocolTest() override;

private slots:
    void testTransHeaderCreation();
    void testTransHeaderSerialization();
    void testTransHeaderDeserialization();
    void testMagicValidation();
    void testChecksumValidation();
    void testCommandTypes();
    void testProtocolBodies();
    void testNetworkByteOrder();
};

} // namespace Tests
