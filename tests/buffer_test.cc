#include <mymuduo/Buffer.h>
#include "test_assert.h"

#include <string>

void testBuffer()
{
    Buffer buffer;
    TEST_CHECK(buffer.readableBytes() == 0);
    TEST_CHECK(buffer.writableBytes() == Buffer::kInitialSize);

    const std::string first("hello");
    buffer.append(first.data(), first.size());
    TEST_CHECK(buffer.readableBytes() == first.size());
    TEST_CHECK(buffer.retrieveAsString(2) == "he");
    TEST_CHECK(buffer.retrieveAllAsString() == "llo");

    const std::string large(4096, 'x');
    buffer.append(large.data(), large.size());
    TEST_CHECK(buffer.retrieveAllAsString() == large);
}
