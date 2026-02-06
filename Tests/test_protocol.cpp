#include "test_protocol.h"
#include "../../Common/Protocol/CommonProtocol.h"
#include <QtTest>

namespace Tests {

ProtocolTest::ProtocolTest(QObject *parent) : QObject(parent) {}

ProtocolTest::~ProtocolTest() {}

void ProtocolTest::testTransHeader() {
  ::ChatSystem::Protocol::TransHeader header;
  // header.clear(); // TransHeader has no clear(), constructor inits.

  header.cmd = ::ChatSystem::Protocol::CMD_META_LOGIN_REQ;
  header.len = 100;
  header.seq = 1;

  QCOMPARE(header.cmd,
           static_cast<uint32_t>(::ChatSystem::Protocol::CMD_META_LOGIN_REQ));
  QCOMPARE(header.len, static_cast<uint32_t>(100));
  QCOMPARE(header.seq, static_cast<uint32_t>(1));
}

void ProtocolTest::testEndianness() {
  ::ChatSystem::Protocol::TransHeader header;
  header.len = 0x12345678;

  header.toNetworkOrder();
  // Verify network order

  header.toHostOrder();
  QCOMPARE(header.len, static_cast<uint32_t>(0x12345678));
}

} // namespace Tests
