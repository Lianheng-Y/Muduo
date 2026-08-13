#include <mymuduo/EventLoop.h>
#include <mymuduo/EventLoopThread.h>
#include <mymuduo/InetAddress.h>
#include <mymuduo/TcpConnection.h>
#include "test_assert.h"

#include <sys/socket.h>
#include <unistd.h>

#include <future>
#include <string>

void testTcpConnectionCrossThreadSend()
{
    int sockets[2];
    TEST_CHECK(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);

    EventLoopThread loopThread;
    EventLoop* loop = loopThread.startLoop();
    TcpConnectionPtr connection(new TcpConnection(
        loop, "cross-thread-test", sockets[0], InetAddress(), InetAddress()));

    std::promise<void> established;
    loop->runInLoop([connection, &established]() {
        connection->connectEstablished();
        established.set_value();
    });
    established.get_future().wait();

    const std::string expected(8192, 'x');
    connection->send(std::string(expected));

    std::string received(expected.size(), '\0');
    size_t offset = 0;
    while (offset < received.size())
    {
        const ssize_t count = ::read(sockets[1], &received[offset], received.size() - offset);
        TEST_CHECK(count > 0);
        offset += static_cast<size_t>(count);
    }
    TEST_CHECK(received == expected);

    std::promise<void> destroyed;
    loop->runInLoop([connection, &destroyed]() {
        connection->connectDestroyed();
        destroyed.set_value();
    });
    destroyed.get_future().wait();
    connection.reset();
    ::close(sockets[1]);
}
