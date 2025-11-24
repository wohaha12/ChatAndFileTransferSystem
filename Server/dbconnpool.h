#ifndef DBCONNPOOL_H
#define DBCONNPOOL_H

#include <QObject>
#include <QSqlDatabase>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QString>

class DBConnPool : public QObject
{
    Q_OBJECT

private:
    explicit DBConnPool(QObject *parent = nullptr);
    ~DBConnPool();

public:
    // 获取单例实例
    static DBConnPool &getInstance();

    // 初始化连接池
    void init(const QString &hostName, int port, const QString &databaseName,
              const QString &userName, const QString &password, int maxSize = 10, int minSize = 2);

    // 获取数据库连接
    QSqlDatabase getConnection();

    // 释放数据库连接
    void releaseConnection(const QSqlDatabase &conn);

    // 关闭所有连接
    void closeAllConnections();

private:
    // 创建新连接
    QSqlDatabase createConnection();

private:
    QString m_hostName;
    int m_port;
    QString m_databaseName;
    QString m_userName;
    QString m_password;
    int m_maxSize;
    int m_minSize;
    int m_currentSize;

    QQueue<QSqlDatabase> m_freeConnections;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;

    bool m_initialized;
};

#endif // DBCONNPOOL_H
