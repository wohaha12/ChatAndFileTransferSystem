#ifndef CLIENT_MODELS_USERMODEL_H
#define CLIENT_MODELS_USERMODEL_H

#include <QObject>
#include <QString>
#include <QDateTime>

struct UserInfo {
    uint64_t userId;
    QString username;
    QString nickname;
    QString avatarUrl;
    int status;
    QDateTime createTime;
    QDateTime updateTime;

    UserInfo()
        : userId(0)
        , status(0)
    {
    }
};

class UserModel : public QObject
{
    Q_OBJECT

public:
    explicit UserModel(QObject* parent = nullptr);
    ~UserModel();

    void setUserInfo(const UserInfo& info);
    UserInfo getUserInfo() const;

    void setUserId(uint64_t userId);
    uint64_t getUserId() const;

    void setUsername(const QString& username);
    QString getUsername() const;

    void setNickname(const QString& nickname);
    QString getNickname() const;

    void setAvatarUrl(const QString& avatarUrl);
    QString getAvatarUrl() const;

    void setStatus(int status);
    int getStatus() const;

    void clear();

signals:
    void userInfoChanged(const UserInfo& info);
    void usernameChanged(const QString& username);
    void nicknameChanged(const QString& nickname);
    void avatarUrlChanged(const QString& avatarUrl);
    void statusChanged(int status);

private:
    UserInfo m_userInfo;
};

#endif // CLIENT_MODELS_USERMODEL_H
