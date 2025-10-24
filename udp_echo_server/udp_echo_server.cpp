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
 * @file udp_echo_server.cpp
 * @brief UDP Echo Server Sample
 *
 * This sample demonstrates:
 * - Creating a UDP server with messaging_udp_server
 * - Receiving datagrams from clients
 * - Echoing received data back to sender
 * - Handling errors gracefully
 * - Proper server shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <csignal>
#include <thread>
#include <chrono>

#include "network_system/core/messaging_udp_server.h"

using namespace network_system::core;

// Global flag for graceful shutdown
std::atomic<bool> should_stop{false};

/**
 * @brief Signal handler for graceful shutdown
 */
void signal_handler(int signal)
{
    std::cout << "\n[Server] Received signal " << signal << ", shutting down...\n";
    should_stop.store(true);
}

int main(int argc, char* argv[])
{
    std::cout << "=================================================\n";
    std::cout << "  UDP Echo Server Sample\n";
    std::cout << "=================================================\n\n";

    // Parse port from command line (default: 5555)
    uint16_t port = 5555;
    if (argc > 1)
    {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::cout << "[Server] Starting UDP echo server on port " << port << "...\n";

    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try
    {
        // Create UDP server
        auto server = std::make_shared<messaging_udp_server>("UDPEchoServer");

        // Set up receive callback - echo messages back
        server->set_receive_callback(
            [server](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender)
            {
                // Convert received data to string for display
                std::string message(data.begin(), data.end());

                std::cout << "[Server] Received " << data.size() << " bytes from "
                          << sender.address().to_string() << ":" << sender.port()
                          << " - Message: \"" << message << "\"\n";

                // Create echo response
                std::string echo_msg = "Echo: " + message;
                std::vector<uint8_t> echo_data(echo_msg.begin(), echo_msg.end());

                // Send echo back to client
                server->async_send_to(
                    std::move(echo_data),
                    sender,
                    [sender](std::error_code ec, std::size_t bytes_sent)
                    {
                        if (!ec)
                        {
                            std::cout << "[Server] Sent echo response (" << bytes_sent
                                      << " bytes) to " << sender.address().to_string()
                                      << ":" << sender.port() << "\n";
                        }
                        else
                        {
                            std::cerr << "[Server] Failed to send echo: "
                                      << ec.message() << "\n";
                        }
                    });
            });

        // Set up error callback
        server->set_error_callback(
            [](std::error_code ec)
            {
                std::cerr << "[Server] Error occurred: " << ec.message() << "\n";
            });

        // Start the server
        auto result = server->start_server(port);
        if (result.is_err())
        {
            std::cerr << "[Server] Failed to start server: "
                      << result.error().message << "\n";
            return 1;
        }

        std::cout << "[Server] UDP echo server is running on port " << port << "\n";
        std::cout << "[Server] Waiting for datagrams... (Press Ctrl+C to stop)\n\n";

        // Main loop - wait for shutdown signal
        while (!should_stop.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Graceful shutdown
        std::cout << "\n[Server] Stopping server...\n";
        server->stop_server();
        std::cout << "[Server] Server stopped successfully.\n";

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Server] Exception: " << e.what() << "\n";
        return 1;
    }
}
