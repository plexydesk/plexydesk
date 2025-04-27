#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace social_kit {
class SocketWrapper {
public:
    SocketWrapper();
    ~SocketWrapper();

    bool connect(const std::string& host, int port, bool useSSL);
    int send(const void* data, size_t size);
    int recv(void* buffer, size_t size);
    void close();
    bool is_connected() const { return connected_; }
    bool wait_for_ready(bool read, bool write, int timeout_ms);

private:
    int sock_;
    SSL* ssl_;
    SSL_CTX* ctx_;
    bool useSSL_;
    bool connected_;

    bool set_non_blocking();
    bool connect_socket(const std::string& host, int port);
};
}
