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
 * @file reliable_udp_echo_client.cpp
 * @brief Reliable UDP Echo Client Sample
 *
 * This sample demonstrates:
 * - Creating a reliable UDP client with automatic ACK/NACK
 * - Using reliability_mode::reliable_ordered for guaranteed in-order delivery
 * - Automatic packet retransmission on packet loss
 * - Monitoring reliability statistics (RTT, retransmissions, packet loss)
 * - Configurable congestion window and retry parameters
 * - Proper client shutdown
 */

#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

#include "network_system/core/reliable_udp_client.h"

using namespace network_system::core;

// Statistics tracking
std::atomic<int> packets_sent{0};
std::atomic<int> packets_received{0};

int main(int argc, char* argv[])
{
    std::cout << "=================================================\n";
    std::cout << "  Reliable UDP Echo Client Sample\n";
    std::cout << "=================================================\n\n";

    // Parse server address from command line (default: localhost:7777)
    std::string server_host = "127.0.0.1";
    uint16_t server_port = 7777;

    if (argc > 1)
    {
        server_host = argv[1];
    }
    if (argc > 2)
    {
        server_port = static_cast<uint16_t>(std::atoi(argv[2]));
    }

    std::cout << "[Client] Connecting to " << server_host << ":" << server_port << "...\n";
    std::cout << "[Client] Using reliability_mode::reliable_ordered\n";
    std::cout << "[Client] Features: ACK/NACK, automatic retransmission, in-order delivery\n\n";

    try
    {
        // Create reliable UDP client with reliable_ordered mode
        auto client = std::make_shared<reliable_udp_client>(
            "ReliableUDPEchoClient",
            reliability_mode::reliable_ordered);

        // Configure reliability parameters
        client->set_congestion_window(32);          // Max 32 unacked packets
        client->set_max_retries(5);                 // Retry up to 5 times
        client->set_retransmission_timeout(200);    // 200ms timeout

        // Set up receive callback - display echo responses
        client->set_receive_callback(
            [](const std::vector<uint8_t>& data)
            {
                std::string response(data.begin(), data.end());
                std::cout << "[Client] Received reliable response (" << data.size()
                          << " bytes): \"" << response << "\"\n";
                packets_received++;
            });

        // Set up error callback
        client->set_error_callback(
            [](std::error_code ec)
            {
                std::cerr << "[Client] Error occurred: " << ec.message() << "\n";
            });

        // Start the client
        auto result = client->start_client(server_host, server_port);
        if (result.is_err())
        {
            std::cerr << "[Client] Failed to start client: "
                      << result.error().message << "\n";
            return 1;
        }

        std::cout << "[Client] Reliable UDP client started\n";
        std::cout << "[Client] Sending echo packets...\n\n";

        // Send 5 test packets with reliability
        const int num_packets = 5;
        for (int i = 1; i <= num_packets; ++i)
        {
            std::string message = "Reliable packet #" + std::to_string(i);
            std::vector<uint8_t> data(message.begin(), message.end());

            std::cout << "[Client] Sending packet #" << i << ": \"" << message << "\"\n";

            auto send_result = client->send_packet(std::move(data));
            if (send_result.is_ok())
            {
                packets_sent++;
            }
            else
            {
                std::cerr << "[Client] Failed to send packet: "
                          << send_result.error().message << "\n";
            }

            // Wait before sending next packet
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // Display statistics after each packet
            auto stats = client->get_stats();
            std::cout << "[Stats] Sent: " << stats.packets_sent
                      << ", Received: " << stats.packets_received
                      << ", Retransmitted: " << stats.packets_retransmitted
                      << ", ACKs: " << stats.acks_received
                      << ", RTT: " << stats.average_rtt_ms << "ms\n\n";
        }

        // Wait for remaining responses
        std::cout << "[Client] Waiting for remaining responses...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Display final statistics
        auto final_stats = client->get_stats();
        std::cout << "\n=================================================\n";
        std::cout << "  Final Statistics\n";
        std::cout << "=================================================\n";
        std::cout << "Application level:\n";
        std::cout << "  Packets sent:     " << packets_sent.load() << "\n";
        std::cout << "  Packets received: " << packets_received.load() << "\n";
        std::cout << "  Success rate:     "
                  << (packets_sent.load() > 0
                          ? (packets_received.load() * 100 / packets_sent.load())
                          : 0)
                  << "%\n\n";

        std::cout << "Reliability layer:\n";
        std::cout << "  Total sent:       " << final_stats.packets_sent << "\n";
        std::cout << "  Total received:   " << final_stats.packets_received << "\n";
        std::cout << "  Retransmitted:    " << final_stats.packets_retransmitted << "\n";
        std::cout << "  Dropped:          " << final_stats.packets_dropped << "\n";
        std::cout << "  ACKs sent:        " << final_stats.acks_sent << "\n";
        std::cout << "  ACKs received:    " << final_stats.acks_received << "\n";
        std::cout << "  Average RTT:      " << final_stats.average_rtt_ms << " ms\n";
        std::cout << "  Reliability mode: reliable_ordered\n";
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
