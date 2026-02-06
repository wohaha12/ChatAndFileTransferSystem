#include "HashUtil.h"
#include <QCryptographicHash>
#include <QFile>
#include <QRandomGenerator>
#include <iomanip>
#include <sstream>

namespace ChatSystem {
namespace Utils {

std::string HashUtil::calculateMD5(const std::string &input) {
  QByteArray data = QByteArray::fromRawData(input.c_str(), input.length());
  QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
  return hash.toHex().toStdString();
}

bool HashUtil::calculateFileMD5(const std::string &filePath,
                                std::string &md5Result) {
  QFile file(QString::fromStdString(filePath));
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QCryptographicHash hash(QCryptographicHash::Md5);
  if (hash.addData(&file)) {
    md5Result = hash.result().toHex().toStdString();
    return true;
  }
  return false;
}

std::string HashUtil::calculateSHA256(const std::string &input) {
  QByteArray data = QByteArray::fromRawData(input.c_str(), input.length());
  QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
  return hash.toHex().toStdString();
}

bool HashUtil::calculateFileSHA256(const std::string &filePath,
                                   std::string &sha256Result) {
  QFile file(QString::fromStdString(filePath));
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QCryptographicHash hash(QCryptographicHash::Sha256);
  if (hash.addData(&file)) {
    sha256Result = hash.result().toHex().toStdString();
    return true;
  }
  return false;
}

std::string HashUtil::generateSalt(size_t length) {
  const char charset[] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::string salt;
  salt.reserve(length);
  for (size_t i = 0; i < length; ++i) {
    salt += charset[QRandomGenerator::global()->bounded(
        static_cast<int>(sizeof(charset) - 1))];
  }
  return salt;
}

std::string HashUtil::calculatePasswordHash(const std::string &password,
                                            const std::string &salt) {
  return calculateSHA256(password + salt);
}

} // namespace Utils
} // namespace ChatSystem
