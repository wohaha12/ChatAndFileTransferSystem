#include "TcpServer.h"
#include "Connection.h"
#include "../../Common/Protocol/TransHeader.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

namespace ChatSystem {
namespace StorageServer {
namespace Network {

TcpServer::TcpServer(QObject* parent)
    : QObject(parent)
    , m_epollFd(-1)
    , m_listenFd(-1)
    , m_eventThread(nullptr)
    , m_running(false)
    , m_connectionCount(0)
{
}

TcpServer::~TcpServer()
{
    stop();
    
    for (auto& pair : m_connections) {
        delete pair.second;
    }
    m_connections.clear();
    
    delete m_eventThread;
}

bool TcpServer::start(const std::string& ip, uint16_t port)
{
    if (m_running) {
        std::cerr << "服务器已在运行" << std::endl;
        return false;
    }
    
    m_listenFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_listenFd < 0) {
        std::cerr << "创建套接字失败" << std::endl;
        return false;
    }
    
    int opt = 1;
    setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    
    if (bind(m_listenFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "绑定套接字失败" << std::endl;
        close(m_listenFd);
        return false;
    }
    
    if (listen(m_listenFd, 1024) < 0) {
        std::cerr << "监听套接字失败" << std::endl;
        close(m_listenFd);
        return false;
    }
    
    m_epollFd = epoll_create1(0);
    if (m_epollFd < 0) {
        std::cerr << "创建epoll失败" << std::endl;
        close(m_listenFd);
        return false;
    }
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = m_listenFd;
    
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_listenFd, &ev) < 0) {
        std::cerr << "添加监听套接字到epoll失败" << std::endl;
        close(m_epollFd);
        close(m_listenFd);
        return false;
    }
    
    m_running = true;
    m_eventThread = new QThread();
    m_eventThread->setStackSize(1024 * 1024);
    
    connect(m_eventThread, &QThread::started, this, &TcpServer::eventLoop);
    connect(m_eventThread, &QThread::finished, m_eventThread, &QThread::deleteLater);
    
    m_eventThread->start();
    
    std::cout << "Epoll TCP服务器启动成功: " << ip << ":" << port << std::endl;
    return true;
}

void TcpServer::stop()
{
    if (!m_running) {
        return;
    }
    
    m_running = false;
    
    if (m_eventThread && m_eventThread->isRunning()) {
        m_eventThread->quit();
        m_eventThread->wait();
    }
    
    for (auto& pair : m_connections) {
        delete pair.second;
    }
    m_connections.clear();
    
    if (m_epollFd >= 0) {
        close(m_epollFd);
        m_epollFd = -1;
    }
    
    if (m_listenFd >= 0) {
        close(m_listenFd);
        m_listenFd = -1;
    }
    
    std::cout << "Epoll TCP服务器已停止" << std::endl;
}

bool TcpServer::isRunning() const
{
    return m_running;
}

int TcpServer::getConnectionCount() const
{
    return m_connectionCount;
}

void TcpServer::eventLoop()
{
    const int MAX_EVENTS = 1024;
    struct epoll_event events[MAX_EVENTS];
    
    while (m_running) {
        int nfds = epoll_wait(m_epollFd, events, MAX_EVENTS, 1000);
        
        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "epoll_wait失败: " << strerror(errno) << std::endl;
            break;
        }
        
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == m_listenFd) {
                handleNewConnection();
            } else {
                if (events[i].events & EPOLLIN) {
                    handleReadableEvent(events[i].data.fd);
                }
                
                if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    handleConnectionClose(events[i].data.fd);
                }
            }
        }
    }
}

void TcpServer::handleNewConnection()
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    
    int clientFd = accept(m_listenFd, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd < 0) {
        std::cerr << "接受连接失败" << std::endl;
        return;
    }
    
    int flags = fcntl(clientFd, F_GETFL, 0);
    fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
    
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    ev.data.fd = clientFd;
    
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0) {
        std::cerr << "添加客户端套接字到epoll失败" << std::endl;
        close(clientFd);
        return;
    }
    
    Connection* connection = new Connection(clientFd, this);
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections[clientFd] = connection;
        m_connectionCount++;
    }
    
    emit newConnection(clientFd);
    std::cout << "新连接到达: " << clientFd << std::endl;
}

void TcpServer::handleConnectionClose(qintptr socketDescriptor)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_connections.find(socketDescriptor);
    if (it != m_connections.end()) {
        delete it->second;
        m_connections.erase(it);
        m_connectionCount--;
        
        emit connectionClosed(socketDescriptor);
        std::cout << "连接关闭: " << socketDescriptor << std::endl;
    }
    
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, socketDescriptor, nullptr);
    close(socketDescriptor);
}

void TcpServer::handleReadableEvent(qintptr socketDescriptor)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_connections.find(socketDescriptor);
    if (it != m_connections.end()) {
        Connection* connection = it->second;
        connection->processReceivedData();
    }
}

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem