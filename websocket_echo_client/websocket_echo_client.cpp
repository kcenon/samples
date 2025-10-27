/*****************************************************************************\
BSD 3-Clause License

Copyright (c) 2024, kcenon
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\*****************************************************************************/

/**
 * @file websocket_echo_client.cpp
 * @brief WebSocket Echo Client Sample
 *
 * This sample demonstrates:
 * - Creating a WebSocket client
 * - Performing WebSocket handshake
 * - Sending text and binary WebSocket messages
 * - Receiving echo responses from server
 * - Handling connection/disconnection events
 * - Proper client shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

#include "network_system/core/messaging_ws_client.h"

using namespace network_system::core;

// Statistics tracking
std::atomic<int> messages_sent{0};
std::atomic<int> messages_received{0};

int main(int argc, char* argv[])
{
    std::cout << "=================================================\n";
    std::cout << "  WebSocket Echo Client Sample\n";
    std::cout << "=================================================\n\n";

    // Parse server address from command line (default: localhost:8080/ws)
    std::string server_host = "127.0.0.1";
    uint16_t server_port = 8080;
    std::string path = "/ws";

    if (argc > 1)
    {
        server_host = argv[1];
    }
    if (argc > 2)
    {
        server_port = static_cast<uint16_t>(std::atoi(argv[2]));
    }
    if (argc > 3)
    {
        path = argv[3];
    }

    std::cout << "[Client] Connecting to WebSocket server ws://" << server_host
              << ":" << server_port << path << "...\n";

    try
    {
        // Create WebSocket client
        auto client = std::make_shared<messaging_ws_client>("WSEchoClient");

        // Set up connected callback
        client->set_connected_callback(
            []()
            {
                std::cout << "[Client] Connected to WebSocket server (handshake complete)\n";
            });

        // Set up disconnected callback
        client->set_disconnected_callback(
            [](auto close_code, const std::string& reason)
            {
                std::cout << "[Client] Disconnected from WebSocket server";
                if (!reason.empty())
                {
                    std::cout << " (reason: " << reason << ")";
                }
                std::cout << "\n";
            });

        // Set up text message callback - display text responses
        client->set_text_message_callback(
            [](const std::string& message)
            {
                std::cout << "[Client] Received text response (" << message.size()
                          << " bytes): \"" << message << "\"\n";
                messages_received++;
            });

        // Set up binary message callback - display binary responses
        client->set_binary_message_callback(
            [](const std::vector<uint8_t>& data)
            {
                std::cout << "[Client] Received binary response (" << data.size()
                          << " bytes)\n";
                messages_received++;
            });

        // Set up error callback
        client->set_error_callback(
            [](std::error_code ec)
            {
                std::cerr << "[Client] Error occurred: " << ec.message() << "\n";
            });

        // Start the client and connect to WebSocket server
        auto result = client->start_client(server_host, server_port, path);
        if (result.is_err())
        {
            std::cerr << "[Client] Failed to start client: "
                      << result.error().message << "\n";
            return 1;
        }

        // Wait for connection to establish and WebSocket handshake to complete
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (!client->is_connected())
        {
            std::cerr << "[Client] Failed to connect to WebSocket server\n";
            std::cerr << "[Client] Please ensure:\n";
            std::cerr << "  1. Server is running\n";
            std::cerr << "  2. Server is listening on port " << server_port << "\n";
            std::cerr << "  3. Server WebSocket path is " << path << "\n";
            return 1;
        }

        std::cout << "[Client] Sending WebSocket echo messages...\n\n";

        // Send 3 text messages
        for (int i = 1; i <= 3; ++i)
        {
            std::string message = "WebSocket message #" + std::to_string(i);

            std::cout << "[Client] Sending text message #" << i << ": \"" << message << "\"\n";

            auto send_result = client->send_text(std::move(message),
                [](std::error_code ec, std::size_t bytes_sent)
                {
                    if (!ec)
                    {
                        std::cout << "[Client] Text message sent (" << bytes_sent << " bytes)\n";
                    }
                    else
                    {
                        std::cerr << "[Client] Failed to send text message: " << ec.message() << "\n";
                    }
                });

            if (send_result.is_ok())
            {
                messages_sent++;
            }
            else
            {
                std::cerr << "[Client] Failed to queue text message: "
                          << send_result.error().message << "\n";
            }

            // Wait for response before sending next message
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Send 2 binary messages
        std::cout << "\n[Client] Sending binary messages...\n";
        for (int i = 1; i <= 2; ++i)
        {
            std::vector<uint8_t> binary_data(100, static_cast<uint8_t>(i));
            std::cout << "[Client] Sending binary message #" << i
                      << " (" << binary_data.size() << " bytes)\n";

            auto send_result = client->send_binary(std::move(binary_data),
                [](std::error_code ec, std::size_t bytes_sent)
                {
                    if (!ec)
                    {
                        std::cout << "[Client] Binary message sent (" << bytes_sent << " bytes)\n";
                    }
                    else
                    {
                        std::cerr << "[Client] Failed to send binary message: " << ec.message() << "\n";
                    }
                });

            if (send_result.is_ok())
            {
                messages_sent++;
            }
            else
            {
                std::cerr << "[Client] Failed to queue binary message: "
                          << send_result.error().message << "\n";
            }

            // Wait for response
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Wait a bit more for final responses
        std::cout << "\n[Client] Waiting for remaining responses...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Display statistics
        std::cout << "\n=================================================\n";
        std::cout << "  Statistics\n";
        std::cout << "=================================================\n";
        std::cout << "Messages sent:     " << messages_sent.load() << "\n";
        std::cout << "Messages received: " << messages_received.load() << "\n";
        std::cout << "Success rate:      "
                  << (messages_sent.load() > 0
                          ? (messages_received.load() * 100 / messages_sent.load())
                          : 0)
                  << "%\n";
        std::cout << "Protocol:          WebSocket (ws://)\n";
        std::cout << "=================================================\n\n";

        // Stop the client
        std::cout << "[Client] Stopping WebSocket client...\n";
        auto stop_result = client->stop_client();
        if (stop_result.is_ok())
        {
            std::cout << "[Client] Client stopped successfully.\n";
        }
        else
        {
            std::cerr << "[Client] Error stopping client: "
                      << stop_result.error().message << "\n";
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Client] Exception: " << e.what() << "\n";
        return 1;
    }
}
