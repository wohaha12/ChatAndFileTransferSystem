#include "test_protocol.h"
#include "../../Common/Protocol/TransHeader.h"
#include <QtTest>
#include <QByteArray>

namespace Tests {

ProtocolTest::ProtocolTest(QObject* parent)
    : QObject(parent)
{
}

ProtocolTest::~ProtocolTest()
{
}

void ProtocolTest::testTransHeaderCreation()
{
    ChatSystem::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    header.checksum = 0;
    
    QVERIFY(header.validateMagic());
    QCOMPARE(header.version, static_cast<uint8_t>(0x01));
    QCOMPARE(header.command, static_cast<uint8_t>(0x01));
}

void ProtocolTest::testTransHeaderSerialization()
{
    ChatSystem::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    header.checksum = 0;
    
    header.toNetworkOrder();
    
    QByteArray data;
    data.resize(sizeof(ChatSystem::Protocol::TransHeader));
    memcpy(data.data(), &header, sizeof(ChatSystem::Protocol::TransHeader));
    
    QCOMPARE(data.size(), static_cast<int>(sizeof(ChatSystem::Protocol::TransHeader)));
}

void ProtocolTest::testTransHeaderDeserialization()
{
    ChatSystem::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    header.checksum = 0;
    
    header.toNetworkOrder();
    
    QByteArray data;
    data.resize(sizeof(ChatSystem::Protocol::TransHeader));
    memcpy(data.data(), &header, sizeof(ChatSystem::Protocol::TransHeader));
    
    ChatSystem::Protocol::TransHeader parsedHeader;
    memcpy(&parsedHeader, data.constData(), sizeof(ChatSystem::Protocol::TransHeader));
    parsedHeader.toHostOrder();
    
    QCOMPARE(parsedHeader.magic, header.magic);
    QCOMPARE(parsedHeader.version, header.version);
    QCOMPARE(parsedHeader.command, header.command);
    QCOMPARE(parsedHeader.sequence, header.sequence);
    QCOMPARE(parsedHeader.len, header.len);
}

void ProtocolTest::testMagicValidation()
{
    ChatSystem::Protocol::TransHeader validHeader;
    validHeader.magic = 0x4D53424D;
    
    QVERIFY(validHeader.validateMagic());
    
    ChatSystem::Protocol::TransHeader invalidHeader;
    invalidHeader.magic = 0x12345678;
    
    QVERIFY(!invalidHeader.validateMagic());
}

void ProtocolTest::testChecksumValidation()
{
    ChatSystem::Protocol::TransHeader header;
    header.magic = 0x4D53424D;
    header.version = 0x01;
    header.command = 0x01;
    header.sequence = 0x00000001;
    header.len = 100;
    
    QByteArray testData("Hello, World!");
    header.calculateChecksum(testData.constData(), testData.size());
    
    QVERIFY(header.validateChecksum(testData.constData()));
    
    ChatSystem::Protocol::TransHeader invalidHeader;
    invalidHeader.magic = 0x4D53424D;
    invalidHeader.version = 0x01;
    invalidHeader.command = 0x01;
    invalidHeader.sequence = 0x00000001;
    invalidHeader.len = 100;
    invalidHeader.checksum = 0xFFFFFFFF;
    
    QVERIFY(!invalidHeader.validateChecksum(testData.constData()));
}

void ProtocolTest::testCommandTypes()
{
    QCOMPARE(static_cast<int>(ChatSystem::Protocol::CommandType::REGISTER), 0x01);
    QCOMPARE(static_cast<int>(ChatSystem::Protocol::CommandType::LOGIN), 0x02);
    QCOMPARE(static_cast<int>(ChatSystem::Protocol::CommandType::LOGOUT), 0x03);
    QCOMPARE(static_cast<int>(ChatSystem::Protocol::CommandType::UPLOAD_START), 0x10);
    QCOMPARE(static_cast<int>(ChatSystem::Protocol::CommandType::UPLOAD_DATA), 0x11);
    QCOMPARE(static_cast<int>(ChatSystem::Protocol::CommandType::UPLOAD_END), 0x12);
}

} // namespace Tests