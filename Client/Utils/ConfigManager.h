#ifndef CLIENT_UTILS_CONFIGMANAGER_H
#define CLIENT_UTILS_CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariant>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject* parent = nullptr);
    ~ConfigManager();

    bool loadConfig(const QString& configPath);
    bool saveConfig(const QString& configPath);

    void setValue(const QString& key, const QVariant& value);
    QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    void setMetaServerIp(const QString& ip);
    QString getMetaServerIp() const;

    void setMetaServerPort(uint16_t port);
    uint16_t getMetaServerPort() const;

    void setStorageServerIp(const QString& ip);
    QString getStorageServerIp() const;

    void setStorageServerPort(uint16_t port);
    uint16_t getStorageServerPort() const;

    void setUsername(const QString& username);
    QString getUsername() const;

    void setPassword(const QString& password);
    QString getPassword() const;

    void setAutoLogin(bool autoLogin);
    bool getAutoLogin() const;

    void setRememberPassword(bool remember);
    bool getRememberPassword() const;

    void setDownloadPath(const QString& path);
    QString getDownloadPath() const;

    void setUploadPath(const QString& path);
    QString getUploadPath() const;

    void clear();

signals:
    void configChanged(const QString& key);

private:
    QSettings* m_settings;
    QString m_configPath;
};

#endif // CLIENT_UTILS_CONFIGMANAGER_H
