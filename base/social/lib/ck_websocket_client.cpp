#include "ck_websocket_client.h"
#include <regex>
#include <sstream>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

namespace social_kit {

std::string random_sec_websocket_key()
{
    unsigned char key[16];
    std::random_device rd;
    for (int i = 0; i < 16; ++i) {
        key[i] = rd();
    }

    // base64 encode (simple)
    static const char* base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string base64;
    for (int i = 0; i < 16; i += 3) {
        int val = (key[i] << 16) + (key[i + 1] << 8) + key[i + 2];
        base64.push_back(base64_chars[(val >> 18) & 0x3F]);
        base64.push_back(base64_chars[(val >> 12) & 0x3F]);
        base64.push_back(base64_chars[(val >> 6) & 0x3F]);
        base64.push_back(base64_chars[val & 0x3F]);
    }
    return base64;
}


WebSocketClient::WebSocketClient()
    : running_(false), port_(80), use_ssl_(false)
{
}

WebSocketClient::~WebSocketClient()
{
    if (is_running()) {
    	close();
    }
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
    if (!running_) return;
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

    send_pending(); // Immediately flush queued messages

    auto last_ping = std::chrono::steady_clock::now();

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

        // Send PING every 30 seconds
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_ping).count() >= 30)
        {
            send_ping();
            last_ping = now;
        }
    }

    if (on_close_) on_close_();
}

void WebSocketClient::send_ping()
{
    std::string frame;
    frame += static_cast<char>(0x89); // 0x8 = control frame, 0x9 = PING
    frame += static_cast<char>(0x00); // Payload length = 0
    socket_.send(frame.c_str(), frame.size());
}


bool WebSocketClient::perform_handshake()
{
    std::string request = build_handshake_request();
    socket_.send(request.c_str(), request.size());

    std::string response;
    char buffer[1024];

    auto start_time = std::chrono::steady_clock::now();

    while (true)
    {
        int len = socket_.recv(buffer, sizeof(buffer));
        if (len > 0)
        {
            response.append(buffer, len);
            if (response.find("\r\n\r\n") != std::string::npos) {
                break; // Full HTTP headers received
            }
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 5)
        {
            if (on_error_) on_error_("Handshake timeout");
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (response.find("101 Switching Protocols") != std::string::npos) {
        return true;
    }

    if (on_error_) on_error_("Handshake failed: " + response);
    return false;
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

        if (!fin) {
            fragmented_message_buffer_ += payload;
            continue;
        } else if (!fragmented_message_buffer_.empty()) {
            fragmented_message_buffer_ += payload;
            payload = fragmented_message_buffer_;
            fragmented_message_buffer_.clear();
        }

        if (opcode == 1 && on_message_) {
            on_message_(payload);
        }

        read_buffer_.erase(0, header_size + mask_offset + payload_len);
    }
}

std::string WebSocketClient::build_handshake_request()
{
    std::ostringstream ss;
    ss << "GET " << path_ << " HTTP/1.1\r\n"
       << "Host: " << host_ << ":" << port_ << "\r\n"
       << "Upgrade: websocket\r\n"
       << "Connection: Upgrade\r\n"
       << "Sec-WebSocket-Key: " << random_sec_websocket_key() << "\r\n"
       << "Sec-WebSocket-Version: 13\r\n\r\n";
    return ss.str();
}

std::string WebSocketClient::encode_frame(const std::string& message)
{
    std::string frame;
    frame += 0x81; // FIN + Text frame

    size_t payload_len = message.size();

    if (payload_len <= 125) {
        frame += static_cast<char>(0x80 | payload_len); // MASK bit set
    } else if (payload_len <= 65535) {
        frame += static_cast<char>(0x80 | 126);
        frame += static_cast<char>((payload_len >> 8) & 0xFF);
        frame += static_cast<char>(payload_len & 0xFF);
    } else {
        frame += static_cast<char>(0x80 | 127);
        for (int i = 7; i >= 0; --i)
            frame += static_cast<char>((payload_len >> (i * 8)) & 0xFF);
    }

    // Create random 4-byte mask
    uint8_t masking_key[4];
    std::random_device rd;
    for (int i = 0; i < 4; ++i) {
        masking_key[i] = rd() & 0xFF;
        frame += masking_key[i];
    }

    // Mask the payload
    for (size_t i = 0; i < payload_len; ++i) {
        frame += message[i] ^ masking_key[i % 4];
    }

    return frame;
}


void WebSocketClient::shutdown()
{
    running_ = false;
    socket_.close();
}

bool WebSocketClient::is_running() const
{
    return running_;
}


} // namespace social_kit
