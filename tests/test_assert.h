#pragma once

#include <cstdlib>
#include <iostream>

#define TEST_CHECK(condition) \
    do \
    { \
        if (!(condition)) \
        { \
            std::cerr << __FILE__ << ':' << __LINE__ \
                      << ": check failed: " #condition << std::endl; \
            std::abort(); \
        } \
    } while (false)
