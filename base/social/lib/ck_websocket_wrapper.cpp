#include "ck_websocket_wrapper.h"

#ifdef _WIN32
    using socket_t = SOCKET;
#else
    using socket_t = int;
#endif

namespace social_kit {

SocketWrapper::SocketWrapper()
    : sock_(-1), ssl_(nullptr), ctx_(nullptr), useSSL_(false), connected_(false)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}

SocketWrapper::~SocketWrapper()
{
    close();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool SocketWrapper::connect(const std::string& host, int port, bool useSSL)
{
    useSSL_ = useSSL;
    if (!connect_socket(host, port)) return false;

    if (useSSL_)
    {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        ctx_ = SSL_CTX_new(TLS_client_method());
        if (!ctx_) return false;

        ssl_ = SSL_new(ctx_);
        SSL_set_fd(ssl_, sock_);
        SSL_set_tlsext_host_name(ssl_, host.c_str());

        // SET BLOCKING TEMPORARILY
#ifdef _WIN32
        u_long blocking_mode = 0;
        ioctlsocket(sock_, FIONBIO, &blocking_mode);
#else
        int flags = fcntl(sock_, F_GETFL, 0);
        fcntl(sock_, F_SETFL, flags & ~O_NONBLOCK);
#endif

        int ret;
        do {
            ret = SSL_connect(ssl_);
            if (ret <= 0)
            {
                int err = SSL_get_error(ssl_, ret);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
                {
                    ERR_print_errors_fp(stderr);
                    return false;
                }
            }
        } while (ret != 1);

        // AFTER SSL CONNECT, SET NON-BLOCKING BACK
#ifdef _WIN32
        u_long nonblocking_mode = 1;
        ioctlsocket(sock_, FIONBIO, &nonblocking_mode);
#else
        flags = fcntl(sock_, F_GETFL, 0);
        fcntl(sock_, F_SETFL, flags | O_NONBLOCK);
#endif
    }
    else
    {
        std::cout << "Socket connected without SSL\n";
    }

    connected_ = true;
    return true;
}

bool SocketWrapper::connect_socket(const std::string& host, int port)
{
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0)
    {
        std::cerr << "getaddrinfo failed for host: " << host << "\n";
        return false;
    }

    sock_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_ < 0)
    {
        std::cerr << "socket() creation failed\n";
        freeaddrinfo(res);
        return false;
    }

    if (!set_non_blocking())
    {
        std::cerr << "set_non_blocking() failed\n";
        freeaddrinfo(res);
        return false;
    }

    int connect_ret = ::connect(sock_, res->ai_addr, (int)res->ai_addrlen);

#ifdef _WIN32
    int last_error = WSAGetLastError();
#else
    int last_error = errno;
#endif

    freeaddrinfo(res);

    if (connect_ret == 0)
    {
        // Connected immediately
        return true;
    }

#ifdef _WIN32
    if (last_error == WSAEWOULDBLOCK)
    {
        if (wait_for_ready(false, true, 5000)) // wait up to 5 seconds for writable
        {
            return true;
        }
        else
        {
            std::cerr << "connect() timeout after WSAEWOULDBLOCK\n";
            return false;
        }
    }
#else
    if (last_error == EINPROGRESS)
    {
        if (wait_for_ready(false, true, 5000))
        {
            int sock_error = 0;
            socklen_t len = sizeof(sock_error);
            if (getsockopt(sock_, SOL_SOCKET, SO_ERROR, &sock_error, &len) < 0 || sock_error != 0)
            {
                std::cerr << "connect() failed after select(), errno=" << sock_error << " (" << strerror(sock_error) << ")\n";
                return false;
            }
            return true;
        }
        else
        {
            std::cerr << "connect() timeout after EINPROGRESS\n";
            return false;
        }
    }
#endif

    std::cerr << "connect() failed immediately, error = " << last_error << "\n";
    return false;
}


bool SocketWrapper::set_non_blocking()
{
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(sock_, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(sock_, F_GETFL, 0);
    return fcntl(sock_, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}

int SocketWrapper::send(const void* data, size_t size)
{
    if (!connected_) return -1;
    if (useSSL_) {
        return SSL_write(ssl_, data, (int)size);
    } else {
        return ::send(sock_, (const char*)data, (int)size, 0);
    }
}

int SocketWrapper::recv(void* buffer, size_t size)
{
    if (!connected_) return -1;
    if (useSSL_) {
        return SSL_read(ssl_, buffer, (int)size);
    } else {
        return ::recv(sock_, (char*)buffer, (int)size, 0);
    }
}

bool SocketWrapper::wait_for_ready(bool want_read, bool want_write, int timeout_ms)
{
    fd_set readfds;
    fd_set writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    if (want_read) FD_SET(sock_, &readfds);
    if (want_write) FD_SET(sock_, &writefds);

    timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    return select(sock_ + 1, &readfds, &writefds, nullptr, &tv) > 0;
}

void SocketWrapper::close()
{
    if (!connected_) return; 

    connected_ = false;

    if (ssl_)
    {
        SSL_shutdown(static_cast<SSL*>(ssl_));
        SSL_free(static_cast<SSL*>(ssl_));
        ssl_ = nullptr;
    }
    if (ctx_)
    {
        SSL_CTX_free(static_cast<SSL_CTX*>(ctx_));
        ctx_ = nullptr;
    }

#ifdef _WIN32
    closesocket(sock_);
#else
    ::close(sock_);
#endif
}
}
