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
 * @file websocket_echo_server.cpp
 * @brief WebSocket Echo Server Sample
 *
 * This sample demonstrates:
 * - Creating a WebSocket server
 * - Handling WebSocket handshake
 * - Receiving text and binary WebSocket messages
 * - Echoing messages back to clients
 * - Broadcasting messages to all connected clients
 * - Managing multiple WebSocket connections
 * - Proper server shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <csignal>
#include <thread>
#include <chrono>

#include "network_system/core/messaging_ws_server.h"

using namespace network_system::core;

// Global flag for graceful shutdown
std::atomic<bool> should_stop{false};

// Statistics
std::atomic<int> total_connections{0};
std::atomic<int> active_connections{0};
std::atomic<int> messages_received{0};

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
    std::cout << "  WebSocket Echo Server Sample\n";
    std::cout << "=================================================\n\n";

    // Parse port from command line (default: 8080)
    uint16_t port = 8080;
    std::string path = "/ws";

    if (argc > 1)
    {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }
    if (argc > 2)
    {
        path = argv[2];
    }

    std::cout << "[Server] Starting WebSocket echo server on port " << port << "...\n";
    std::cout << "[Server] WebSocket path: " << path << "\n";
    std::cout << "[Server] Connect using: ws://localhost:" << port << path << "\n\n";

    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try
    {
        // Create WebSocket server
        auto server = std::make_shared<messaging_ws_server>("WSEchoServer");

        // Set up connection callback
        server->set_connection_callback(
            [](auto connection)
            {
                total_connections++;
                active_connections++;
                std::cout << "[Server] New WebSocket connection: " << connection->connection_id()
                          << " from " << connection->remote_endpoint() << "\n";
                std::cout << "[Server] Active connections: " << active_connections.load() << "\n";
            });

        // Set up disconnection callback
        server->set_disconnection_callback(
            [](const std::string& conn_id, auto close_code, const std::string& reason)
            {
                active_connections--;
                std::cout << "[Server] WebSocket disconnected: " << conn_id;
                if (!reason.empty())
                {
                    std::cout << " (reason: " << reason << ")";
                }
                std::cout << "\n";
                std::cout << "[Server] Active connections: " << active_connections.load() << "\n";
            });

        // Set up text message callback - echo text messages
        server->set_text_message_callback(
            [](auto connection, const std::string& message)
            {
                messages_received++;
                std::cout << "[Server] Received text message (" << message.size() << " bytes) from "
                          << connection->connection_id() << ": \"" << message << "\"\n";

                // Echo back with prefix
                std::string echo_msg = "Echo: " + message;
                auto result = connection->send_text(std::move(echo_msg),
                    [](std::error_code ec, std::size_t bytes_sent)
                    {
                        if (!ec)
                        {
                            std::cout << "[Server] Sent text echo (" << bytes_sent << " bytes)\n";
                        }
                        else
                        {
                            std::cerr << "[Server] Failed to send text echo: " << ec.message() << "\n";
                        }
                    });

                if (result.is_err())
                {
                    std::cerr << "[Server] Failed to queue text message: "
                              << result.error().message << "\n";
                }
            });

        // Set up binary message callback - echo binary messages
        server->set_binary_message_callback(
            [](auto connection, const std::vector<uint8_t>& data)
            {
                messages_received++;
                std::cout << "[Server] Received binary message (" << data.size() << " bytes) from "
                          << connection->connection_id() << "\n";

                // Echo back binary data
                std::vector<uint8_t> echo_data = data; // Copy data
                auto result = connection->send_binary(std::move(echo_data),
                    [](std::error_code ec, std::size_t bytes_sent)
                    {
                        if (!ec)
                        {
                            std::cout << "[Server] Sent binary echo (" << bytes_sent << " bytes)\n";
                        }
                        else
                        {
                            std::cerr << "[Server] Failed to send binary echo: " << ec.message() << "\n";
                        }
                    });

                if (result.is_err())
                {
                    std::cerr << "[Server] Failed to queue binary message: "
                              << result.error().message << "\n";
                }
            });

        // Set up error callback
        server->set_error_callback(
            [](const std::string& conn_id, std::error_code ec)
            {
                std::cerr << "[Server] Error on connection " << conn_id << ": "
                          << ec.message() << "\n";
            });

        // Start the WebSocket server
        auto result = server->start_server(port, path);
        if (result.is_err())
        {
            std::cerr << "[Server] Failed to start server: "
                      << result.error().message << "\n";
            return 1;
        }

        std::cout << "[Server] WebSocket echo server is running\n";
        std::cout << "[Server] Waiting for connections... (Press Ctrl+C to stop)\n\n";

        // Main loop - wait for shutdown signal
        while (!should_stop.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Display final statistics
        std::cout << "\n=================================================\n";
        std::cout << "  Statistics\n";
        std::cout << "=================================================\n";
        std::cout << "Total connections:  " << total_connections.load() << "\n";
        std::cout << "Active connections: " << active_connections.load() << "\n";
        std::cout << "Messages received:  " << messages_received.load() << "\n";
        std::cout << "=================================================\n\n";

        // Graceful shutdown
        std::cout << "[Server] Stopping WebSocket server...\n";
        auto stop_result = server->stop_server();
        if (stop_result.is_ok())
        {
            std::cout << "[Server] Server stopped successfully.\n";
        }
        else
        {
            std::cerr << "[Server] Error stopping server: "
                      << stop_result.error().message << "\n";
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Server] Exception: " << e.what() << "\n";
        return 1;
    }
}
