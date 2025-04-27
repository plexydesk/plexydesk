cmake_minimum_required(VERSION 3.10)
project(SocialKitWebSocketClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(wsclient
    sample_client.cpp
    ../ck_websocket_client.cpp
    ../ck_websocket_wrapper.cpp
)

target_link_libraries(wsclient PRIVATE OpenSSL::SSL OpenSSL::Crypto pthread)
