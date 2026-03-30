#include <mymuduo/TcpServer.h>
#include <mymuduo/Logger.h>
#include <mymuduo/AsyncLogging.h>
#include <string>
#include <functional>
#include <memory>

static std::unique_ptr<AsyncLogging> g_asyncLog;

void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void asyncFlush()
{
    g_asyncLog->stop();
}

class EchoServer{
public:
    EchoServer(EventLoop *loop, const InetAddress &addr, const std::string &name) : server_(loop, addr, name), loop_(loop) {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1)
        );
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        server_.setThreadNum(3);
    }
    void start() {
        server_.start();
    }
private:
    void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            LOG_INFO("connection up : %s", conn->peerAddress().toIpPort().c_str());
        } else {
            LOG_INFO("connection down : %s", conn->peerAddress().toIpPort().c_str());
        }
    }
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();
    }

    EventLoop *loop_;
    TcpServer server_;
};

int main() {
    g_asyncLog.reset(new AsyncLogging("EchoServer", 500*1024*1024));
    Logger::setOutput(asyncOutput);
    Logger::setFlush(asyncFlush);
    g_asyncLog->start();

    EventLoop loop;
    InetAddress addr(8000);
    EchoServer server(&loop, addr, "EchoServer-01");
    server.start();
    loop.loop();

    return 0;
}
