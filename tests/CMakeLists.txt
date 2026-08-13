add_executable(mymuduo_tests
    async_logging_test.cc
    buffer_test.cc
    inet_address_test.cc
    tcp_connection_test.cc
    timestamp_test.cc
)
target_link_libraries(mymuduo_tests PRIVATE mymuduo::mymuduo)
if(MYMUDUO_ENABLE_WARNINGS)
    target_compile_options(mymuduo_tests PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    )
endif()
add_test(NAME mymuduo.unit COMMAND mymuduo_tests)
