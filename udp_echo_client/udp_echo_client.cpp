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
 * @file udp_echo_client.cpp
 * @brief UDP Echo Client Sample
 *
 * This sample demonstrates:
 * - Creating a UDP client with messaging_udp_client
 * - Sending datagrams to a server
 * - Receiving echo responses
 * - Handling errors gracefully
 * - Proper client shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

#include "network_system/core/messaging_udp_client.h"

using namespace network_system::core;

// Counter for tracking responses
std::atomic<int> messages_sent{0};
std::atomic<int> responses_received{0};

int main(int argc, char* argv[])
{
    std::cout << "=================================================\n";
    std::cout << "  UDP Echo Client Sample\n";
    std::cout << "=================================================\n\n";

    // Parse server address from command line (default: localhost:5555)
    std::string host = "localhost";
    uint16_t port = 5555;

    if (argc > 1)
    {
        host = argv[1];
    }
    if (argc > 2)
    {
        port = static_cast<uint16_t>(std::atoi(argv[2]));
    }

    std::cout << "[Client] Connecting to UDP server at " << host << ":" << port << "...\n";

    try
    {
        // Create UDP client
        auto client = std::make_shared<messaging_udp_client>("UDPEchoClient");

        // Set up receive callback - handle echo responses
        client->set_receive_callback(
            [](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender)
            {
                std::string message(data.begin(), data.end());
                std::cout << "[Client] Received response: \"" << message << "\"\n";
                std::cout << "[Client] From: " << sender.address().to_string()
                          << ":" << sender.port() << "\n\n";

                responses_received++;
            });

        // Set up error callback
        client->set_error_callback(
            [](std::error_code ec)
            {
                std::cerr << "[Client] Error occurred: " << ec.message() << "\n";
            });

        // Start the client
        auto result = client->start_client(host, port);
        if (result.is_err())
        {
            std::cerr << "[Client] Failed to start client: "
                      << result.error().message << "\n";
            return 1;
        }

        std::cout << "[Client] Connected to " << host << ":" << port << "\n\n";

        // Test messages to send
        const std::vector<std::string> test_messages = {
            "Hello, UDP Server!",
            "This is message #2",
            "Testing UDP echo",
            "UDP is fast and efficient",
            "Final test message"
        };

        // Send messages
        std::cout << "[Client] Sending " << test_messages.size() << " test messages...\n\n";

        for (const auto& msg : test_messages)
        {
            std::cout << "[Client] Sending: \"" << msg << "\"\n";

            std::vector<uint8_t> data(msg.begin(), msg.end());

            auto send_result = client->send_packet(
                std::move(data),
                [msg](std::error_code ec, std::size_t bytes_sent)
                {
                    if (!ec)
                    {
                        std::cout << "[Client] Sent \"" << msg << "\" ("
                                  << bytes_sent << " bytes)\n";
                    }
                    else
                    {
                        std::cerr << "[Client] Failed to send message: "
                                  << ec.message() << "\n";
                    }
                });

            if (send_result.is_err())
            {
                std::cerr << "[Client] Send error: " << send_result.error().message << "\n";
            }
            else
            {
                messages_sent++;
            }

            // Wait between messages
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Wait for all responses
        std::cout << "\n[Client] Waiting for responses...\n";

        int wait_count = 0;
        while (responses_received.load() < messages_sent.load() && wait_count < 50)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            wait_count++;
        }

        // Display statistics
        std::cout << "\n=================================================\n";
        std::cout << "  Results\n";
        std::cout << "=================================================\n";
        std::cout << "Messages Sent:      " << messages_sent.load() << "\n";
        std::cout << "Responses Received: " << responses_received.load() << "\n";
        std::cout << "=================================================\n\n";

        // Shutdown
        std::cout << "[Client] Stopping client...\n";
        client->stop_client();
        std::cout << "[Client] Client stopped successfully.\n";

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Client] Exception: " << e.what() << "\n";
        return 1;
    }
}
