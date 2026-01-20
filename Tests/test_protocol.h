#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

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
};

} // namespace Tests