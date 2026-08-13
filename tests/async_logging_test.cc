#include <mymuduo/AsyncLogging.h>
#include "test_assert.h"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

void testAsyncLogging()
{
    const std::string path("/tmp/mymuduo_async_logging_test.log");
    std::remove(path.c_str());

    const std::string expected("flush-on-stop\n");
    {
        AsyncLogging logging(path, 1024 * 1024, 60);
        logging.start();
        logging.append(expected.data(), static_cast<int>(expected.size()));
        logging.stop();
    }

    std::ifstream input(path.c_str());
    const std::string actual((std::istreambuf_iterator<char>(input)),
                             std::istreambuf_iterator<char>());
    TEST_CHECK(actual == expected);
    std::remove(path.c_str());
}
