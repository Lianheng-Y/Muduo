#include <mymuduo/InetAddress.h>
#include "test_assert.h"

void testBuffer();
void testAsyncLogging();
void testTcpConnectionCrossThreadSend();
void testTimestamp();

int main()
{
    testBuffer();
    testAsyncLogging();

    InetAddress address(8080, "127.0.0.1");
    TEST_CHECK(address.toIp() == "127.0.0.1");
    TEST_CHECK(address.toPort() == 8080);
    TEST_CHECK(address.toIpPort() == "127.0.0.1:8080");

    testTcpConnectionCrossThreadSend();
    testTimestamp();
    return 0;
}
