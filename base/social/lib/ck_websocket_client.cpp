#include "ck_websocket_client.h"
#include <regex>
#include <sstream>
#include <iostream>

namespace social_kit {

WebSocketClient::WebSocketClient()
    : running_(false), port_(80), use_ssl_(false)
{
}

WebSocketClient::~WebSocketClient()
{
    close();
}

bool WebSocketClient::connect(const std::string& url)
{
    std::regex url_re(R"(^(ws|wss)://([^/:]+)(:(\d+))?(/.*)?$)");
    std::smatch match;
    if (!std::regex_match(url, match, url_re)) {
        if (on_error_) on_error_("Invalid URL format");
        return false;
    }

    use_ssl_ = match[1] == "wss";
    host_ = match[2];
    port_ = match[4].matched ? std::stoi(match[4]) : (use_ssl_ ? 443 : 80);
    path_ = match[5].matched ? std::string(match[5]) : "/";

    if (!socket_.connect(host_, port_, use_ssl_)) {
        if (on_error_) on_error_("Socket connection failed");
        return false;
    }

    running_ = true;
    io_thread_ = std::thread(&WebSocketClient::event_loop, this);

    return true;
}

void WebSocketClient::send(const std::string& message)
{
    std::lock_guard<std::mutex> lock(send_queue_mutex_);
    send_queue_.push(encode_frame(message));
}

void WebSocketClient::close()
{
    running_ = false;
    socket_.close();
    if (io_thread_.joinable()) io_thread_.join();
}

void WebSocketClient::event_loop()
{
    if (!perform_handshake()) {
        shutdown();
        return;
    }

    if (on_connect_) on_connect_();

    while (running_)
    {
        bool want_read = true;
        bool want_write = false;
        {
            std::lock_guard<std::mutex> lock(send_queue_mutex_);
            want_write = !send_queue_.empty();
        }

        if (!socket_.wait_for_ready(want_read, want_write, 1000)) {
            continue;
        }

        if (want_read) read_data();
        if (want_write) send_pending();
    }

    if (on_close_) on_close_();
}

bool WebSocketClient::perform_handshake()
{
    std::string request = build_handshake_request();
    socket_.send(request.c_str(), request.size());

    char buffer[4096];
    int len = socket_.recv(buffer, sizeof(buffer));
    if (len <= 0) return false;

    std::string response(buffer, len);
    return response.find("101 Switching Protocols") != std::string::npos;
}

void WebSocketClient::send_pending()
{
    std::lock_guard<std::mutex> lock(send_queue_mutex_);
    while (!send_queue_.empty()) {
        std::string& data = send_queue_.front();
        int sent = socket_.send(data.c_str(), data.size());
        if (sent <= 0) {
            shutdown();
            return;
        }
        send_queue_.pop();
    }
}

void WebSocketClient::read_data()
{
    char buffer[4096];
    int len = socket_.recv(buffer, sizeof(buffer));
    if (len <= 0) {
        shutdown();
        return;
    }
    read_buffer_.append(buffer, len);
    parse_frames();
}

void WebSocketClient::parse_frames()
{
    while (read_buffer_.size() >= 2)
    {
        bool fin = (read_buffer_[0] & 0x80) != 0;
        uint8_t opcode = read_buffer_[0] & 0x0F;
        bool masked = (read_buffer_[1] & 0x80) != 0;
        uint64_t payload_len = read_buffer_[1] & 0x7F;
        size_t header_size = 2;

        if (payload_len == 126) {
            if (read_buffer_.size() < 4) return;
            payload_len = ((unsigned char)read_buffer_[2] << 8) | (unsigned char)read_buffer_[3];
            header_size += 2;
        } else if (payload_len == 127) {
            if (read_buffer_.size() < 10) return;
            for (int i = 2; i < 10; ++i) {
                payload_len = (payload_len << 8) | (unsigned char)read_buffer_[i];
            }
            header_size += 8;
        }

        size_t mask_offset = masked ? 4 : 0;
        if (read_buffer_.size() < header_size + mask_offset + payload_len) return;

        std::string payload;
        if (masked) {
            char mask[4];
            for (int i = 0; i < 4; ++i) mask[i] = read_buffer_[header_size + i];
            for (size_t i = 0; i < payload_len; ++i) {
                payload += read_buffer_[header_size + mask_offset + i] ^ mask[i % 4];
            }
        } else {
            payload = read_buffer_.substr(header_size, payload_len);
        }

        if (opcode == 1 && on_message_) {
            on_message_(payload);
        }

        read_buffer_.erase(0, header_size + mask_offset + payload_len);

        if (!fin) {
            // TODO: handle fragmentation later if needed
            break;
        }
    }
}

std::string WebSocketClient::build_handshake_request()
{
    std::ostringstream ss;
    ss << "GET " << path_ << " HTTP/1.1\r\n"
       << "Host: " << host_ << ":" << port_ << "\r\n"
       << "Upgrade: websocket\r\n"
       << "Connection: Upgrade\r\n"
       << "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"
       << "Sec-WebSocket-Version: 13\r\n\r\n";
    return ss.str();
}

std::string WebSocketClient::encode_frame(const std::string& message)
{
    std::string frame;
    frame += 0x81; // FIN + Text Frame

    if (message.size() <= 125) {
        frame += static_cast<char>(message.size());
    } else if (message.size() <= 65535) {
        frame += 126;
        frame += static_cast<char>((message.size() >> 8) & 0xFF);
        frame += static_cast<char>(message.size() & 0xFF);
    } else {
        frame += 127;
        for (int i = 7; i >= 0; --i)
            frame += static_cast<char>((message.size() >> (i * 8)) & 0xFF);
    }

    frame += message;
    return frame;
}

void WebSocketClient::shutdown()
{
    running_ = false;
    socket_.close();
}

} // namespace social_kit
