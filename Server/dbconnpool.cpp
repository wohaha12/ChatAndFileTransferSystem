#include "dbconnpool.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include <QDateTime>

DBConnPool::DBConnPool(QObject *parent) : QObject(parent)
    , m_hostName("")
    , m_port(3306)
    , m_databaseName("")
    , m_userName("")
    , m_password("")
    , m_maxSize(10)
    , m_minSize(2)
    , m_currentSize(0)
    , m_initialized(false)
{
}

DBConnPool::~DBConnPool()
{
    closeAllConnections();
}

DBConnPool &DBConnPool::getInstance()
{
    static DBConnPool instance;
    return instance;
}

void DBConnPool::init(const QString &hostName, int port, const QString &databaseName,
                     const QString &userName, const QString &password, int maxSize, int minSize)
{
    if (m_initialized) {
        qWarning() << "数据库连接池已经初始化过了";
        return;
    }

    m_hostName = hostName;
    m_port = port;
    m_databaseName = databaseName;
    m_userName = userName;
    m_password = password;
    m_maxSize = maxSize;
    m_minSize = minSize;

    // 创建最小数量的连接
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_minSize; ++i) {
        QSqlDatabase conn = createConnection();
        if (conn.isValid()) {
            m_freeConnections.enqueue(conn);
            m_currentSize++;
        }
    }

    m_initialized = true;
    qDebug() << "数据库连接池初始化成功，当前连接数：" << m_currentSize;
}

QSqlDatabase DBConnPool::getConnection()
{
    if (!m_initialized) {
        qCritical() << "数据库连接池未初始化";
        return QSqlDatabase();
    }

    QMutexLocker locker(&m_mutex);

    // 如果有空闲连接，直接返回
    if (!m_freeConnections.isEmpty()) {
        QSqlDatabase conn = m_freeConnections.dequeue();
        if (conn.isOpen() && conn.isValid()) {
            return conn;
        } else {
            // 连接无效，创建新连接
            qDebug() << "连接无效，创建新连接";
            QSqlDatabase::removeDatabase(conn.connectionName());
            return createConnection();
        }
    }

    // 如果当前连接数小于最大连接数，创建新连接
    if (m_currentSize < m_maxSize) {
        m_currentSize++;
        locker.unlock();
        return createConnection();
    }

    // 等待有连接可用
    qDebug() << "等待数据库连接...";
    m_waitCondition.wait(&m_mutex, 5000); // 超时5秒

    if (!m_freeConnections.isEmpty()) {
        QSqlDatabase conn = m_freeConnections.dequeue();
        if (conn.isOpen() && conn.isValid()) {
            return conn;
        } else {
            QSqlDatabase::removeDatabase(conn.connectionName());
            m_currentSize--;
            return createConnection();
        }
    }

    qCritical() << "获取数据库连接超时";
    return QSqlDatabase();
}

void DBConnPool::releaseConnection(const QSqlDatabase &conn)
{
    if (!conn.isValid()) {
        qWarning() << "释放无效的数据库连接";
        return;
    }

    QMutexLocker locker(&m_mutex);

    // 检查连接是否已经在队列中
    for (const QSqlDatabase &db : m_freeConnections) {
        if (db.connectionName() == conn.connectionName()) {
            qWarning() << "连接已经在队列中，无需重复释放";
            return;
        }
    }

    if (conn.isOpen()) {
        m_freeConnections.enqueue(conn);
        m_waitCondition.wakeOne();
        qDebug() << "数据库连接已释放，当前空闲连接数：" << m_freeConnections.size();
    } else {
        qWarning() << "释放关闭的数据库连接";
        QSqlDatabase::removeDatabase(conn.connectionName());
        m_currentSize--;
    }
}

void DBConnPool::closeAllConnections()
{
    QMutexLocker locker(&m_mutex);

    while (!m_freeConnections.isEmpty()) {
        QSqlDatabase conn = m_freeConnections.dequeue();
        if (conn.isOpen()) {
            conn.close();
        }
        QSqlDatabase::removeDatabase(conn.connectionName());
    }

    m_currentSize = 0;
    m_initialized = false;
    qDebug() << "所有数据库连接已关闭";
}

QSqlDatabase DBConnPool::createConnection()
{
    // 使用线程地址和时间戳生成唯一连接名称
    QString connectionName = QString("Connection_%1_%2").arg(reinterpret_cast<quintptr>(QThread::currentThread())).arg(QDateTime::currentMSecsSinceEpoch());

    QSqlDatabase conn = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    conn.setHostName(m_hostName);
    conn.setPort(m_port);
    conn.setDatabaseName(m_databaseName);
    conn.setUserName(m_userName);
    conn.setPassword(m_password);

    // 设置连接属性
    conn.setConnectOptions("MYSQL_OPT_RECONNECT=1"); // 自动重连

    if (!conn.open()) {
        qCritical() << "创建数据库连接失败：" << conn.lastError().text()
                    << "连接名称：" << connectionName;
        QSqlDatabase::removeDatabase(connectionName);
        return QSqlDatabase();
    }

    qDebug() << "创建数据库连接成功：" << connectionName;
    return conn;
}
