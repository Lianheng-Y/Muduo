#include <mymuduo/Timestamp.h>
#include "test_assert.h"

void testTimestamp()
{
    const Timestamp now = Timestamp::now();
    TEST_CHECK(now.microSecondsSinceEpoch() > 0);
    TEST_CHECK(!now.toString().empty());
}
