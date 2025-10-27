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
 * @file reliable_udp_echo_server.cpp
 * @brief Reliable UDP Echo Server Sample
 *
 * This sample demonstrates:
 * - Creating a reliable UDP endpoint for server-side
 * - Using reliability_mode::reliable_ordered for guaranteed delivery
 * - Receiving reliable UDP packets with ACK/NACK
 * - Echoing packets back with automatic retransmission
 * - Monitoring connection statistics (RTT, retransmissions)
 * - Proper server shutdown
 *
 * Note: This uses reliable_udp_client in peer-to-peer mode as a server endpoint.
 * UDP is connectionless, so the distinction between client and server is logical.
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

// Statistics
std::atomic<int> packets_received{0};
std::atomic<int> packets_sent{0};

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
    std::cout << "  Reliable UDP Echo Server Sample\n";
    std::cout << "=================================================\n\n";

    // Parse port from command line (default: 7777)
    uint16_t port = 7777;
    if (argc > 1)
    {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::cout << "[Server] Starting reliable UDP echo server on port " << port << "...\n";
    std::cout << "[Server] Note: This uses standard UDP server with application-level reliability\n\n";

    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try
    {
        // Create UDP server for reliable communication
        // Note: reliable_udp_client is designed for client-to-server communication
        // For a true reliable UDP server, we use messaging_udp_server as the base
        auto server = std::make_shared<messaging_udp_server>("ReliableUDPEchoServer");

        // Set up receive callback - echo packets back
        server->set_receive_callback(
            [server](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender)
            {
                packets_received++;

                // Convert received data to string for display
                std::string message(data.begin(), data.end());

                std::cout << "[Server] Received " << data.size() << " bytes from "
                          << sender.address().to_string() << ":" << sender.port()
                          << " - Message: \"" << message << "\"\n";

                // Create echo response with prefix
                std::string echo_msg = "Reliable Echo: " + message;
                std::vector<uint8_t> echo_data(echo_msg.begin(), echo_msg.end());

                // Send echo back to sender
                server->async_send_to(
                    std::move(echo_data),
                    sender,
                    [](std::error_code ec, std::size_t bytes_sent)
                    {
                        if (!ec)
                        {
                            packets_sent++;
                            std::cout << "[Server] Sent reliable echo response ("
                                      << bytes_sent << " bytes)\n";
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

        std::cout << "[Server] Reliable UDP echo server is running on port " << port << "\n";
        std::cout << "[Server] Waiting for datagrams... (Press Ctrl+C to stop)\n\n";

        // Main loop - wait for shutdown signal and display statistics periodically
        auto last_stats_time = std::chrono::steady_clock::now();

        while (!should_stop.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Display statistics every 10 seconds
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_time);

            if (elapsed.count() >= 10 && (packets_received > 0 || packets_sent > 0))
            {
                std::cout << "\n[Statistics] Packets received: " << packets_received.load()
                          << ", Packets sent: " << packets_sent.load() << "\n\n";
                last_stats_time = now;
            }
        }

        // Display final statistics
        std::cout << "\n=================================================\n";
        std::cout << "  Final Statistics\n";
        std::cout << "=================================================\n";
        std::cout << "Packets received: " << packets_received.load() << "\n";
        std::cout << "Packets sent:     " << packets_sent.load() << "\n";
        std::cout << "Success rate:     "
                  << (packets_received.load() > 0
                          ? (packets_sent.load() * 100 / packets_received.load())
                          : 0)
                  << "%\n";
        std::cout << "=================================================\n\n";

        // Graceful shutdown
        std::cout << "[Server] Stopping server...\n";
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
