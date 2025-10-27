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
 * @file secure_echo_server.cpp
 * @brief Secure TCP Echo Server Sample (TLS/SSL)
 *
 * This sample demonstrates:
 * - Creating a secure TCP server with TLS/SSL encryption
 * - Loading SSL certificates and private keys
 * - Handling encrypted client connections
 * - Receiving and sending encrypted messages
 * - Handling TLS handshake errors
 * - Proper secure server shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <csignal>
#include <thread>
#include <chrono>
#include <filesystem>

#include "network_system/core/secure_messaging_server.h"
#include "network_system/session/secure_session.h"

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
    std::cout << "  Secure TCP Echo Server Sample (TLS/SSL)\n";
    std::cout << "=================================================\n\n";

    // Parse command line arguments
    uint16_t port = 9877;
    std::string cert_file = "server.crt";
    std::string key_file = "server.key";

    if (argc > 1)
    {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }
    if (argc > 2)
    {
        cert_file = argv[2];
    }
    if (argc > 3)
    {
        key_file = argv[3];
    }

    // Check if certificate and key files exist
    if (!std::filesystem::exists(cert_file))
    {
        std::cerr << "[Server] Certificate file not found: " << cert_file << "\n";
        std::cerr << "[Server] Please generate certificates using: ./generate_cert.sh\n";
        return 1;
    }

    if (!std::filesystem::exists(key_file))
    {
        std::cerr << "[Server] Key file not found: " << key_file << "\n";
        std::cerr << "[Server] Please generate certificates using: ./generate_cert.sh\n";
        return 1;
    }

    std::cout << "[Server] Using certificate: " << cert_file << "\n";
    std::cout << "[Server] Using private key: " << key_file << "\n";
    std::cout << "[Server] Starting secure TCP echo server on port " << port << "...\n";

    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try
    {
        // Create secure TCP server with TLS/SSL
        auto server = std::make_shared<secure_messaging_server>(
            "SecureTCPEchoServer", cert_file, key_file);

        // Set up connection callback - log new secure connections
        server->set_connection_callback(
            [](auto session)
            {
                std::cout << "[Server] New secure client connected (TLS handshake complete)\n";
            });

        // Set up disconnection callback - log disconnections
        server->set_disconnection_callback(
            [](const std::string& session_id)
            {
                std::cout << "[Server] Client disconnected: " << session_id << "\n";
            });

        // Set up receive callback - echo encrypted messages back
        server->set_receive_callback(
            [](auto session, const std::vector<uint8_t>& data)
            {
                // Convert received data to string for display
                std::string message(data.begin(), data.end());

                std::cout << "[Server] Received " << data.size() << " bytes (encrypted)"
                          << " - Message: \"" << message << "\"\n";

                // Create echo response
                std::string echo_msg = "Secure Echo: " + message;
                std::vector<uint8_t> echo_data(echo_msg.begin(), echo_msg.end());

                // Send encrypted echo back to client
                session->send_packet(std::move(echo_data));

                std::cout << "[Server] Sent encrypted echo response (" << echo_msg.size()
                          << " bytes)\n";
            });

        // Set up error callback - handle TLS errors
        server->set_error_callback(
            [](auto session, std::error_code ec)
            {
                std::cerr << "[Server] Error occurred: " << ec.message() << "\n";
                if (ec.category() == asio::error::get_ssl_category())
                {
                    std::cerr << "[Server] SSL/TLS error detected\n";
                }
            });

        // Start the secure server
        auto result = server->start_server(port);
        if (result.is_err())
        {
            std::cerr << "[Server] Failed to start server: "
                      << result.error().message << "\n";
            return 1;
        }

        std::cout << "[Server] Secure TCP echo server is running on port " << port << "\n";
        std::cout << "[Server] All connections are encrypted with TLS/SSL\n";
        std::cout << "[Server] Waiting for secure connections... (Press Ctrl+C to stop)\n\n";

        // Main loop - wait for shutdown signal
        while (!should_stop.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Graceful shutdown
        std::cout << "\n[Server] Stopping secure server...\n";
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
