#include "ConfigManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent)
    , m_settings(nullptr)
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);

    m_configPath = configDir + "/config.ini";
    loadConfig(m_configPath);
}

ConfigManager::~ConfigManager()
{
    if (m_settings) {
        delete m_settings;
    }
}

bool ConfigManager::loadConfig(const QString& configPath)
{
    if (m_settings) {
        delete m_settings;
    }

    m_configPath = configPath;
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);

    qDebug() << "Config loaded from:" << configPath;
    return true;
}

bool ConfigManager::saveConfig(const QString& configPath)
{
    if (!m_settings) {
        return false;
    }

    m_settings->sync();
    qDebug() << "Config saved to:" << configPath;
    return true;
}

void ConfigManager::setValue(const QString& key, const QVariant& value)
{
    if (!m_settings) {
        return;
    }

    m_settings->setValue(key, value);
    m_settings->sync();
    emit configChanged(key);
}

QVariant ConfigManager::getValue(const QString& key, const QVariant& defaultValue) const
{
    if (!m_settings) {
        return defaultValue;
    }

    return m_settings->value(key, defaultValue);
}

void ConfigManager::setMetaServerIp(const QString& ip)
{
    setValue("Network/MetaServerIp", ip);
}

QString ConfigManager::getMetaServerIp() const
{
    return getValue("Network/MetaServerIp", "127.0.0.1").toString();
}

void ConfigManager::setMetaServerPort(uint16_t port)
{
    setValue("Network/MetaServerPort", port);
}

uint16_t ConfigManager::getMetaServerPort() const
{
    return static_cast<uint16_t>(getValue("Network/MetaServerPort", 8888).toUInt());
}

void ConfigManager::setStorageServerIp(const QString& ip)
{
    setValue("Network/StorageServerIp", ip);
}

QString ConfigManager::getStorageServerIp() const
{
    return getValue("Network/StorageServerIp", "127.0.0.1").toString();
}

void ConfigManager::setStorageServerPort(uint16_t port)
{
    setValue("Network/StorageServerPort", port);
}

uint16_t ConfigManager::getStorageServerPort() const
{
    return static_cast<uint16_t>(getValue("Network/StorageServerPort", 9999).toUInt());
}

void ConfigManager::setUsername(const QString& username)
{
    setValue("User/Username", username);
}

QString ConfigManager::getUsername() const
{
    return getValue("User/Username", "").toString();
}

void ConfigManager::setPassword(const QString& password)
{
    setValue("User/Password", password);
}

QString ConfigManager::getPassword() const
{
    return getValue("User/Password", "").toString();
}

void ConfigManager::setAutoLogin(bool autoLogin)
{
    setValue("User/AutoLogin", autoLogin);
}

bool ConfigManager::getAutoLogin() const
{
    return getValue("User/AutoLogin", false).toBool();
}

void ConfigManager::setRememberPassword(bool remember)
{
    setValue("User/RememberPassword", remember);
}

bool ConfigManager::getRememberPassword() const
{
    return getValue("User/RememberPassword", false).toBool();
}

void ConfigManager::setDownloadPath(const QString& path)
{
    setValue("Paths/DownloadPath", path);
}

QString ConfigManager::getDownloadPath() const
{
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    return getValue("Paths/DownloadPath", defaultPath).toString();
}

void ConfigManager::setUploadPath(const QString& path)
{
    setValue("Paths/UploadPath", path);
}

QString ConfigManager::getUploadPath() const
{
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return getValue("Paths/UploadPath", defaultPath).toString();
}

void ConfigManager::clear()
{
    if (!m_settings) {
        return;
    }

    m_settings->clear();
    m_settings->sync();
    qDebug() << "Config cleared";
}
