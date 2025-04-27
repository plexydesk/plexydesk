#ifdef _WIN32
#include <openssl/applink.c>
#endif

#include "../ck_websocket_client.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

using namespace social_kit;

int main()
{
    WebSocketClient client;
    std::atomic<bool> message_received{false};

    client.set_on_connect([&]() {
        std::cout << "Connected to WebSocket!\n";
        client.send("Hello WebSocket from Social Kit!");
    });

    client.set_on_message([&](const std::string& message) {
        std::cout << "Received: " << message << "\n";
        message_received = true;
    });

    client.set_on_close([]() {
        std::cout << "Connection closed.\n";
    });

    client.set_on_error([](const std::string& error) {
        std::cerr << "Error: " << error << "\n";
    });

    if (client.connect("wss://echo.websocket.events")) {
        std::cout << "Connecting...\n";

        // Instead of sleep, wait until message is received
        while (!message_received) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Now close after getting message
        client.close();
    } else {
        std::cerr << "Failed to connect\n";
    }

    return 0;
}
