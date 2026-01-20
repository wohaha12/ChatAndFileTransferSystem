#include "UserModel.h"

UserModel::UserModel(QObject* parent)
    : QObject(parent)
{
}

UserModel::~UserModel()
{
}

void UserModel::setUserInfo(const UserInfo& info)
{
    m_userInfo = info;
    emit userInfoChanged(info);
}

UserInfo UserModel::getUserInfo() const
{
    return m_userInfo;
}

void UserModel::setUserId(uint64_t userId)
{
    if (m_userInfo.userId != userId) {
        m_userInfo.userId = userId;
    }
}

uint64_t UserModel::getUserId() const
{
    return m_userInfo.userId;
}

void UserModel::setUsername(const QString& username)
{
    if (m_userInfo.username != username) {
        m_userInfo.username = username;
        emit usernameChanged(username);
    }
}

QString UserModel::getUsername() const
{
    return m_userInfo.username;
}

void UserModel::setNickname(const QString& nickname)
{
    if (m_userInfo.nickname != nickname) {
        m_userInfo.nickname = nickname;
        emit nicknameChanged(nickname);
    }
}

QString UserModel::getNickname() const
{
    return m_userInfo.nickname;
}

void UserModel::setAvatarUrl(const QString& avatarUrl)
{
    if (m_userInfo.avatarUrl != avatarUrl) {
        m_userInfo.avatarUrl = avatarUrl;
        emit avatarUrlChanged(avatarUrl);
    }
}

QString UserModel::getAvatarUrl() const
{
    return m_userInfo.avatarUrl;
}

void UserModel::setStatus(int status)
{
    if (m_userInfo.status != status) {
        m_userInfo.status = status;
        emit statusChanged(status);
    }
}

int UserModel::getStatus() const
{
    return m_userInfo.status;
}

void UserModel::clear()
{
    m_userInfo = UserInfo();
}
