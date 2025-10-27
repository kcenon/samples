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
 * @file echo_client.cpp
 * @brief TCP Echo Client Sample
 *
 * This sample demonstrates:
 * - Creating a TCP client with messaging_client
 * - Connecting to a remote server
 * - Sending messages to the server
 * - Receiving echo responses from the server
 * - Handling connection/disconnection events
 * - Handling errors gracefully
 * - Proper client shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

#include "network_system/core/messaging_client.h"

using namespace network_system::core;

// Statistics tracking
std::atomic<int> messages_sent{0};
std::atomic<int> messages_received{0};

int main(int argc, char* argv[])
{
    std::cout << "=================================================\n";
    std::cout << "  TCP Echo Client Sample\n";
    std::cout << "=================================================\n\n";

    // Parse server address from command line (default: localhost:9876)
    std::string server_host = "127.0.0.1";
    uint16_t server_port = 9876;

    if (argc > 1)
    {
        server_host = argv[1];
    }
    if (argc > 2)
    {
        server_port = static_cast<uint16_t>(std::atoi(argv[2]));
    }

    std::cout << "[Client] Connecting to " << server_host << ":" << server_port << "...\n";

    try
    {
        // Create TCP client
        auto client = std::make_shared<messaging_client>("TCPEchoClient");

        // Set up connected callback
        client->set_connected_callback(
            []()
            {
                std::cout << "[Client] Connected to server\n";
            });

        // Set up disconnected callback
        client->set_disconnected_callback(
            []()
            {
                std::cout << "[Client] Disconnected from server\n";
            });

        // Set up receive callback - display echo responses
        client->set_receive_callback(
            [](const std::vector<uint8_t>& data)
            {
                std::string response(data.begin(), data.end());
                std::cout << "[Client] Received response (" << data.size() << " bytes): \""
                          << response << "\"\n";
                messages_received++;
            });

        // Set up error callback
        client->set_error_callback(
            [](std::error_code ec)
            {
                std::cerr << "[Client] Error occurred: " << ec.message() << "\n";
            });

        // Start the client and connect to server
        auto result = client->start_client(server_host, server_port);
        if (result.is_err())
        {
            std::cerr << "[Client] Failed to start client: "
                      << result.error().message << "\n";
            return 1;
        }

        // Wait for connection to establish
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (!client->is_connected())
        {
            std::cerr << "[Client] Failed to connect to server\n";
            return 1;
        }

        std::cout << "[Client] Sending echo messages...\n\n";

        // Send 5 test messages
        const int num_messages = 5;
        for (int i = 1; i <= num_messages; ++i)
        {
            std::string message = "Hello from client, message #" + std::to_string(i);
            std::vector<uint8_t> data(message.begin(), message.end());

            std::cout << "[Client] Sending message #" << i << ": \"" << message << "\"\n";

            auto send_result = client->send_packet(std::move(data));
            if (send_result.is_err())
            {
                std::cerr << "[Client] Failed to send message: "
                          << send_result.error().message << "\n";
            }
            else
            {
                messages_sent++;
            }

            // Wait for response before sending next message
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
        std::cout << "=================================================\n\n";

        // Stop the client
        std::cout << "[Client] Stopping client...\n";
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
