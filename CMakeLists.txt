cmake_minimum_required(VERSION 3.16)

project(mymuduo VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(MYMUDUO_BUILD_EXAMPLES "Build mymuduo examples" ON)
option(MYMUDUO_BUILD_TESTS "Build mymuduo tests" ON)
option(MYMUDUO_BUILD_BENCHMARKS "Build mymuduo benchmarks" OFF)
option(MYMUDUO_ENABLE_WARNINGS "Enable additional compiler warnings" ON)

find_package(Threads REQUIRED)

add_library(mymuduo
    src/Acceptor.cc
    src/AsyncLogging.cc
    src/Buffer.cc
    src/Channel.cc
    src/CurrentThread.cc
    src/DefaultPoller.cc
    src/EPollPoller.cc
    src/EventLoop.cc
    src/EventLoopThread.cc
    src/EventLoopThreadPool.cc
    src/InetAddress.cc
    src/Logger.cc
    src/Poller.cc
    src/Socket.cc
    src/TcpConnection.cc
    src/TcpServer.cc
    src/Thread.cc
    src/Timestamp.cc
)
add_library(mymuduo::mymuduo ALIAS mymuduo)

target_compile_features(mymuduo PUBLIC cxx_std_11)
target_include_directories(mymuduo
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/mymuduo
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(mymuduo PUBLIC Threads::Threads)
set_target_properties(mymuduo PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(MYMUDUO_ENABLE_WARNINGS)
    target_compile_options(mymuduo PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    )
endif()

if(MYMUDUO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MYMUDUO_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(MYMUDUO_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

install(TARGETS mymuduo
    EXPORT mymuduoTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT mymuduoTargets
    FILE mymuduoTargets.cmake
    NAMESPACE mymuduo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymuduo
)

configure_package_config_file(
    cmake/mymuduoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/mymuduoConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymuduo
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/mymuduoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mymuduoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/mymuduoConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymuduo
)
