#include "test_protocol.h"
#include "../../Common/Protocol/TransHeader.h"
#include "../../Common/Protocol/CommandTypes.h"
#include "../../Common/Protocol/ProtocolBodies.h"
#include <QtTest>
#include <QByteArray>
#include <cstring>

namespace Tests {

/**
 * @brief 构造函数
 */
ProtocolTest::ProtocolTest(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 */
ProtocolTest::~ProtocolTest()
{
}

/**
 * @brief 测试协议头创建
 * @details 验证协议头能够正确创建和初始化
 */
void ProtocolTest::testTransHeaderCreation()
{
    Common::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    header.checksum = 0;
    
    QVERIFY(header.validateMagic());
    QCOMPARE(header.version, static_cast<uint8_t>(0x01));
    QCOMPARE(header.command, static_cast<uint32_t>(0x01));
}

/**
 * @brief 测试协议头序列化
 * @details 验证协议头能够正确序列化为网络字节序
 */
void ProtocolTest::testTransHeaderSerialization()
{
    Common::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    header.checksum = 0;
    
    header.toNetworkOrder();
    
    QByteArray data;
    data.resize(sizeof(Common::Protocol::TransHeader));
    memcpy(data.data(), &header, sizeof(Common::Protocol::TransHeader));
    
    QCOMPARE(data.size(), static_cast<int>(sizeof(Common::Protocol::TransHeader)));
}

/**
 * @brief 测试协议头反序列化
 * @details 验证协议头能够正确从网络字节序反序列化
 */
void ProtocolTest::testTransHeaderDeserialization()
{
    Common::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    header.checksum = 0;
    
    header.toNetworkOrder();
    
    QByteArray data;
    data.resize(sizeof(Common::Protocol::TransHeader));
    memcpy(data.data(), &header, sizeof(Common::Protocol::TransHeader));
    
    Common::Protocol::TransHeader parsedHeader;
    memcpy(&parsedHeader, data.constData(), sizeof(Common::Protocol::TransHeader));
    parsedHeader.toHostOrder();
    
    QCOMPARE(parsedHeader.magic, header.magic);
    QCOMPARE(parsedHeader.version, header.version);
    QCOMPARE(parsedHeader.command, header.command);
    QCOMPARE(parsedHeader.sequence, header.sequence);
    QCOMPARE(parsedHeader.len, header.len);
}

/**
 * @brief 测试魔数验证
 * @details 验证魔数校验功能
 */
void ProtocolTest::testMagicValidation()
{
    Common::Protocol::TransHeader validHeader;
    validHeader.magic = 0x4D53424D;
    
    QVERIFY(validHeader.validateMagic());
    
    Common::Protocol::TransHeader invalidHeader;
    invalidHeader.magic = 0x12345678;
    
    QVERIFY(!invalidHeader.validateMagic());
}

/**
 * @brief 测试校验和验证
 * @details 验证校验和计算和验证功能
 */
void ProtocolTest::testChecksumValidation()
{
    Common::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    
    QByteArray testData("Hello, World!");
    header.calculateChecksum(testData.constData(), testData.size());
    
    QVERIFY(header.validateChecksum(testData.constData()));
    
    Common::Protocol::TransHeader invalidHeader;
    invalidHeader.magic = 0x4D53424D;
    invalidHeader.version = 0x01;
    invalidHeader.command = 0x01;
    invalidHeader.sequence = 0x00000001;
    invalidHeader.len = 100;
    invalidHeader.checksum = 0xFFFFFFFF;
    
    QVERIFY(!invalidHeader.validateChecksum(testData.constData()));
}

/**
 * @brief 测试命令字类型
 * @details 验证命令字枚举的正确性
 */
void ProtocolTest::testCommandTypes()
{
    QCOMPARE(static_cast<int>(Common::Protocol::CommandType::CMD_META_LOGIN_REQ), 0x1001);
    QCOMPARE(static_cast<int>(Common::Protocol::CommandType::CMD_META_LOGIN_RES), 0x1002);
    QCOMPARE(static_cast<int>(Common::Protocol::CommandType::CMD_META_UPLOAD_REQ), 0x1201);
    QCOMPARE(static_cast<int>(Common::Protocol::CommandType::CMD_META_UPLOAD_RES), 0x1202);
    QCOMPARE(static_cast<int>(Common::Protocol::CommandType::CMD_UPLOAD_CHUNK_REQ), 0x2001);
    QCOMPARE(static_cast<int>(Common::Protocol::CommandType::CMD_UPLOAD_CHUNK_RES), 0x2002);
}

/**
 * @brief 测试协议体
 * @details 验证各种协议体的正确性
 */
void ProtocolTest::testProtocolBodies()
{
    Common::Protocol::LoginReq loginReq;
    strcpy(loginReq.username, "testuser");
    strcpy(loginReq.password_hash, "testhash");
    strcpy(loginReq.client_version, "1.0.0");
    
    QCOMPARE(QString(loginReq.username), QString("testuser"));
    QCOMPARE(QString(loginReq.password_hash), QString("testhash"));
    QCOMPARE(QString(loginReq.client_version), QString("1.0.0"));
    
    Common::Protocol::UploadRequest uploadReq;
    uploadReq.user_id = 12345;
    strcpy(uploadReq.file_name, "testfile.txt");
    uploadReq.file_size = 1024 * 1024;
    strcpy(uploadReq.file_hash, "d41d8cd98f00b204e9800998ecf8427e");
    
    QCOMPARE(uploadReq.user_id, static_cast<uint64_t>(12345));
    QCOMPARE(QString(uploadReq.file_name), QString("testfile.txt"));
    QCOMPARE(uploadReq.file_size, static_cast<uint64_t>(1024 * 1024));
}

/**
 * @brief 测试网络字节序
 * @details 验证网络字节序转换功能
 */
void ProtocolTest::testNetworkByteOrder()
{
    Common::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x1001;
    header.sequence = 0x00000001;
    header.len = 100;
    
    header.toNetworkOrder();
    
    QVERIFY(header.magic != 0x4D53424D);
    
    header.toHostOrder();
    
    QCOMPARE(header.magic, 0x4D53424D);
}

} // namespace Tests
