#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

std::atomic<long long> total_bytes_received(0);
std::atomic<int> active_connections(0);

void client_worker(const char* ip, int port, int connections_per_thread, int msg_size) {
    std::string msg(msg_size, 'a');
    char buf[8192];
    std::vector<int> fds;

    // 1. 建立连接
    for (int i = 0; i < connections_per_thread; ++i) {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        // addr.inet_addr = inet_addr(ip);
        addr.sin_addr.s_addr = inet_addr(ip); 

        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            fds.push_back(fd);
            active_connections++;
        } else {
            close(fd);
        }
    }

    // 2. 开始 Ping-Pong
    while (true) {
        for (int fd : fds) {
            send(fd, msg.c_str(), msg.size(), 0);
            ssize_t n = recv(fd, buf, sizeof(buf), 0);
            if (n > 0) {
                total_bytes_received += n;
            } else if (n == 0) {
                // 连接断开逻辑...
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Usage: %s <ip> <port> <threads> <conns_per_thread> [msg_size]\n", argv[0]);
        return -1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int threads = atoi(argv[3]);
    int conns_per_thread = atoi(argv[4]);
    int msg_size = (argc > 5) ? atoi(argv[5]) : 1024;

    std::vector<std::thread> workers;
    for (int i = 0; i < threads; ++i) {
        workers.emplace_back(client_worker, ip, port, conns_per_thread, msg_size);
    }

    // 3. 每秒打印一次吞吐量
    while (true) {
        long long last_bytes = total_bytes_received.load();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        long long current_bytes = total_bytes_received.load();
        double throughput = (current_bytes - last_bytes) / 1024.0 / 1024.0;
        printf("Active Conns: %d, Throughput: %.2f MiB/s\n", active_connections.load(), throughput);
    }

    return 0;
}