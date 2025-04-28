#pragma once

#include "ck_websocket_wrapper.h"
#include <string>
#include <functional>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>

namespace social_kit {

class WebSocketClient
{
public:
    using OnConnectCallback = std::function<void()>;
    using OnMessageCallback = std::function<void(const std::string&)>;
    using OnCloseCallback = std::function<void()>;
    using OnErrorCallback = std::function<void(const std::string&)>;

    WebSocketClient();
    ~WebSocketClient();

    bool connect(const std::string& url);
    void send(const std::string& message);
    void close();
    bool is_running() const;

    void set_on_connect(OnConnectCallback cb) { on_connect_ = cb; }
    void set_on_message(OnMessageCallback cb) { on_message_ = cb; }
    void set_on_close(OnCloseCallback cb) { on_close_ = cb; }
    void set_on_error(OnErrorCallback cb) { on_error_ = cb; }

private:
    void event_loop();
    bool perform_handshake();
    void send_pending();
    void read_data();
    void parse_frames();
    void send_ping();
    void shutdown();

    std::string build_handshake_request();
    std::string encode_frame(const std::string& message);

    OnConnectCallback on_connect_;
    OnMessageCallback on_message_;
    OnCloseCallback on_close_;
    OnErrorCallback on_error_;

    SocketWrapper socket_;
    std::thread io_thread_;
    std::atomic<bool> running_;
    std::mutex send_queue_mutex_;
    std::queue<std::string> send_queue_;
    std::string read_buffer_;

    std::string host_;
    int port_;
    bool use_ssl_;
    std::string path_;
    std::string fragmented_message_buffer_;
};

} // namespace social_kit
