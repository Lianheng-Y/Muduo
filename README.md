# mymuduo

`mymuduo` 是一个面向 Linux 的 C++11 TCP 网络库，按 Muduo 的核心思路实现了 Reactor：非阻塞 socket、`epoll`、一个基础事件循环加多个 IO 线程，以及线程安全的任务投递。

## 项目结构

```text
include/mymuduo/   对外公开的头文件
src/               库实现
examples/          可运行示例
benchmarks/        压测客户端（可选构建）
tests/             CTest 单元和并发回归测试
cmake/             安装后的 CMake package 配置模板
```

核心对象的关系是：`TcpServer -> Acceptor -> EventLoopThreadPool -> EventLoop -> EPollPoller -> Channel -> TcpConnection`。

## 依赖

- Linux（使用 `epoll`、`eventfd` 和 `accept4`）
- CMake 3.16 或更高版本
- 支持 C++11 的 GCC 或 Clang
- POSIX threads

## 构建和测试

建议使用源码目录外的构建目录：

```bash
cmake -S . -B build -DMYMUDUO_BUILD_BENCHMARKS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

默认构建库、Echo 示例和测试；压测客户端通过 `MYMUDUO_BUILD_BENCHMARKS=ON` 开启。使用 `-DBUILD_SHARED_LIBS=ON` 可以生成共享库，默认生成静态库。

## 运行 Echo 示例

```bash
./build/examples/echo_server
printf 'hello\n' | nc 127.0.0.1 8000
```

示例监听 `127.0.0.1:8000`，按收到的数据回写并保持连接，便于验证多次收发和运行压测客户端。

## 安装和下游项目

```bash
cmake --install build --prefix "$HOME/.local"
```

安装后，下游项目可以使用：

```cmake
find_package(mymuduo CONFIG REQUIRED)
add_executable(app main.cc)
target_link_libraries(app PRIVATE mymuduo::mymuduo)
```

编译时使用 `-DCMAKE_PREFIX_PATH="$HOME/.local"` 指向安装前缀。

## 线程和生命周期约定

- `EventLoop` 只能在创建它的线程中调用 `loop()`，一个线程只能有一个 `EventLoop`。
- `TcpConnection` 的回调在连接所属的 IO 线程执行；`send()` 和 `shutdown()` 可以从其他线程调用。
- 跨线程发送会复制消息内容，并在 IO 线程中执行，调用方不需要保留传入字符串。
- 连接回调、消息回调和关闭回调都是可选的。

## 远程仓库的存储边界

应提交：源码、公开头文件、CMake 文件、测试、示例、README、许可证和必要的 CI 配置。

不应提交：`build/`、编译生成的 `.a/.so`、示例和测试可执行文件、日志、core dump、`perf.data`/火焰图原始采样、个人 IDE 配置。`.gitignore` 已覆盖这些默认产物；性能报告若要长期保留，应提交经过筛选的 Markdown 或图片，而不是机器相关的原始数据。

## 当前边界和后续方向

这是一个 Linux 学习型网络库，不承诺跨平台 ABI。后续可继续增加：连接超时、优雅停机 API、IPv6、更多 Poller 后端、系统错误码封装、Sanitizer/ThreadSanitizer CI，以及更完整的 TCP 分片和背压测试。

## 许可证

仓库目前尚未声明开源许可证。在公开分发或允许第三方复用前，应由仓库所有者选择许可证并提交对应的 `LICENSE` 文件；许可证涉及授权意图，不应由工程化改造代替所有者决定。
