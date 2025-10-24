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
 * @file messaging_integration_sample.cpp
 * @brief Demonstrates messaging_system + logger_system integration
 *
 * This sample shows how to:
 * - Create and configure a message bus
 * - Implement pub/sub pattern with multiple topics
 * - Use request/reply pattern with timeout
 * - Handle message priorities
 * - Monitor messaging statistics
 * - Integrate with logger_system for observability
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <iomanip>
#include <sstream>

#include "fmt/format.h"
#include "kcenon/logger/core/logger.h"
#include "kcenon/logger/writers/console_writer.h"
#include "kcenon/messaging/core/message_bus.h"
#include "kcenon/messaging/core/message_types.h"

using kcenon::logger::logger;
using kcenon::logger::console_writer;
using kcenon::messaging::core::message_bus;
using kcenon::messaging::core::message;
using kcenon::messaging::core::message_bus_config;
using kcenon::messaging::core::message_priority;
using kcenon::messaging::core::message_type;

// Statistics for tracking
struct messaging_statistics {
    std::atomic<uint64_t> total_published{0};
    std::atomic<uint64_t> total_received{0};
    std::atomic<uint64_t> requests_sent{0};
    std::atomic<uint64_t> responses_received{0};
    std::atomic<uint64_t> notifications{0};
    std::atomic<uint64_t> broadcasts{0};
};

// Display message bus statistics
void display_statistics(const message_bus::statistics_snapshot& stats,
                       const messaging_statistics& custom_stats) {
    std::cout << "\n╔═════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Message Bus Statistics Dashboard               ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";

    std::cout << "📊 Message Bus Metrics:\n";
    std::cout << "   Published:         " << stats.messages_published << "\n";
    std::cout << "   Processed:         " << stats.messages_processed << "\n";
    std::cout << "   Failed:            " << stats.messages_failed << "\n";
    std::cout << "   Active Subs:       " << stats.active_subscriptions << "\n";
    std::cout << "   Pending Requests:  " << stats.pending_requests << "\n";

    std::cout << "\n📈 Custom Metrics:\n";
    std::cout << "   Total Published:   " << custom_stats.total_published.load() << "\n";
    std::cout << "   Total Received:    " << custom_stats.total_received.load() << "\n";
    std::cout << "   Requests Sent:     " << custom_stats.requests_sent.load() << "\n";
    std::cout << "   Responses Recv:    " << custom_stats.responses_received.load() << "\n";
    std::cout << "   Notifications:     " << custom_stats.notifications.load() << "\n";
    std::cout << "   Broadcasts:        " << custom_stats.broadcasts.load() << "\n";

    std::cout << "\n═══════════════════════════════════════════════════════════\n\n";
}

// Helper: Create a message with priority
message create_message(const std::string& topic,
                      const std::string& sender,
                      message_priority priority = message_priority::normal) {
    message msg(topic, sender);
    msg.metadata.priority = priority;
    msg.metadata.timestamp = std::chrono::system_clock::now();
    return msg;
}

int main() {
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Messaging Integration Sample\n";
    std::cout << "  (messaging + logger systems)\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // ========================================
    // Phase 1: Initialize Components
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 1: Component Initialization\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // 1. Logger System
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>(true, true));
    log->start();

    log->log(kcenon::thread::log_level::info, "[Logger] Logger system initialized");

    // 2. Message Bus Configuration
    message_bus_config config;
    config.worker_threads = 4;
    config.max_queue_size = 10000;
    config.processing_timeout = std::chrono::milliseconds(30000);
    config.enable_priority_queue = true;
    config.enable_metrics = true;

    // 3. Create and Initialize Message Bus
    auto bus = std::make_unique<message_bus>(config);

    if (!bus->initialize()) {
        log->log(kcenon::thread::log_level::error, "[Message Bus] Initialization failed");
        return 1;
    }

    log->log(kcenon::thread::log_level::info, "[Message Bus] Message bus initialized");
    log->log(kcenon::thread::log_level::info,
            fmt::format("[Message Bus] Worker threads: {}", config.worker_threads));
    log->log(kcenon::thread::log_level::info,
            fmt::format("[Message Bus] Max queue size: {}", config.max_queue_size));

    messaging_statistics stats;

    std::cout << "\n[System] All components initialized successfully\n\n";

    // ========================================
    // Phase 2: Pub/Sub Pattern - Setup Subscribers
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 2: Pub/Sub Pattern - Subscribe to Topics\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Subscriber 1: Orders Topic
    bus->subscribe("orders", [&log, &stats](const message& msg) {
        stats.total_received++;
        log->log(kcenon::thread::log_level::info,
                fmt::format("[Subscriber:Orders] Received order from: {}",
                           msg.metadata.sender));

        auto order_id = msg.payload.get<std::string>("order_id", "unknown");
        auto amount = msg.payload.get<double>("amount", 0.0);

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Subscriber:Orders] Order ID: {}, Amount: ${:.2f}",
                           order_id, amount));
    });

    // Subscriber 2: Inventory Topic
    bus->subscribe("inventory", [&log, &stats](const message& msg) {
        stats.total_received++;
        log->log(kcenon::thread::log_level::info,
                fmt::format("[Subscriber:Inventory] Stock update from: {}",
                           msg.metadata.sender));

        auto item_id = msg.payload.get<std::string>("item_id", "unknown");
        auto quantity = msg.payload.get<int64_t>("quantity", 0);

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Subscriber:Inventory] Item: {}, Quantity: {}",
                           item_id, quantity));
    });

    // Subscriber 3: Notifications Topic
    bus->subscribe("notifications", [&log, &stats](const message& msg) {
        stats.total_received++;
        stats.notifications++;

        auto notification = msg.payload.get<std::string>("message", "");
        log->log(kcenon::thread::log_level::info,
                fmt::format("[Subscriber:Notifications] {}", notification));
    });

    // Subscriber 4: System Broadcasts
    bus->subscribe("system.broadcast", [&log, &stats](const message& msg) {
        stats.total_received++;
        stats.broadcasts++;

        auto announcement = msg.payload.get<std::string>("announcement", "");
        log->log(kcenon::thread::log_level::warning,
                fmt::format("[Subscriber:System] BROADCAST: {}", announcement));
    });

    log->log(kcenon::thread::log_level::info,
            fmt::format("[Phase 2] Subscribed to {} topics",
                       bus->get_topics().size()));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // ========================================
    // Phase 3: Pub/Sub Pattern - Publish Messages
    // ========================================
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 3: Pub/Sub Pattern - Publish Messages\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Publish order messages
    for (int i = 1; i <= 5; ++i) {
        auto msg = create_message("orders", "OrderService");
        msg.payload.set("order_id", fmt::format("ORD-{:04d}", i));
        msg.payload.set("amount", 99.99 * i);
        msg.payload.set("customer", fmt::format("customer-{}", i));

        if (bus->publish(msg)) {
            stats.total_published++;
            log->log(kcenon::thread::log_level::info,
                    fmt::format("[Publisher] Published order message {}", i));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Publish inventory updates
    for (int i = 1; i <= 3; ++i) {
        auto msg = create_message("inventory", "InventoryService", message_priority::high);
        msg.payload.set("item_id", fmt::format("ITEM-{:03d}", i * 10));
        msg.payload.set("quantity", static_cast<int64_t>(100 + i * 50));
        msg.payload.set("warehouse", fmt::format("WH-{}", i));

        if (bus->publish(msg)) {
            stats.total_published++;
            log->log(kcenon::thread::log_level::info,
                    fmt::format("[Publisher] Published inventory update {}", i));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Publish notifications
    for (int i = 1; i <= 3; ++i) {
        auto msg = create_message("notifications", "NotificationService");
        msg.payload.set("message", fmt::format("User action notification #{}", i));
        msg.payload.set("severity", "info");

        if (bus->publish(msg)) {
            stats.total_published++;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Publish system broadcast
    auto broadcast_msg = create_message("system.broadcast", "SystemService",
                                       message_priority::critical);
    broadcast_msg.metadata.type = message_type::broadcast;
    broadcast_msg.payload.set("announcement", "System maintenance scheduled at 2 AM");

    if (bus->publish(broadcast_msg)) {
        stats.total_published++;
        log->log(kcenon::thread::log_level::info, "[Publisher] Published system broadcast");
    }

    std::cout << "\n[Phase 3] Published " << stats.total_published.load()
              << " messages\n";

    // Wait for messages to be processed
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // ========================================
    // Phase 4: Request/Reply Pattern
    // ========================================
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 4: Request/Reply Pattern\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Setup service that responds to requests
    bus->subscribe("service.query", [&log, &bus](const message& request_msg) {
        log->log(kcenon::thread::log_level::info,
                fmt::format("[Service] Received request: {}",
                           request_msg.metadata.id));

        auto query = request_msg.payload.get<std::string>("query", "");
        log->log(kcenon::thread::log_level::info,
                fmt::format("[Service] Query: {}", query));

        // Prepare response
        message response("service.response", "QueryService");
        response.metadata.type = message_type::response;
        response.payload.set("result", fmt::format("Processed: {}", query));
        response.payload.set("status", "success");
        response.payload.set("timestamp", std::chrono::system_clock::now().time_since_epoch().count());

        // Send response
        bus->respond(request_msg, response);

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Service] Sent response for request: {}",
                           request_msg.metadata.id));
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Send requests and wait for responses
    for (int i = 1; i <= 3; ++i) {
        log->log(kcenon::thread::log_level::info,
                fmt::format("[Client] Sending request #{}", i));

        auto request_msg = create_message("service.query", "ClientService");
        request_msg.metadata.type = message_type::request;
        request_msg.metadata.timeout = std::chrono::milliseconds(5000);
        request_msg.payload.set("query", fmt::format("SELECT * FROM table_{}", i));

        stats.requests_sent++;

        try {
            auto future_response = bus->request(request_msg);

            log->log(kcenon::thread::log_level::info,
                    fmt::format("[Client] Request #{} sent, waiting for response...", i));

            // Wait for response with timeout
            if (future_response.wait_for(std::chrono::seconds(3)) == std::future_status::ready) {
                auto response = future_response.get();
                stats.responses_received++;

                auto result = response.payload.get<std::string>("result", "");
                auto status = response.payload.get<std::string>("status", "");

                log->log(kcenon::thread::log_level::info,
                        fmt::format("[Client] Response #{}: {} ({})", i, result, status));
            } else {
                log->log(kcenon::thread::log_level::warning,
                        fmt::format("[Client] Request #{} timed out", i));
            }
        } catch (const std::exception& e) {
            log->log(kcenon::thread::log_level::error,
                    fmt::format("[Client] Request #{} failed: {}", i, e.what()));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\n[Phase 4] Sent " << stats.requests_sent.load()
              << " requests, received " << stats.responses_received.load()
              << " responses\n";

    // ========================================
    // Phase 5: Display Final Statistics
    // ========================================
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 5: Final Statistics\n";
    std::cout << "═══════════════════════════════════════════════════════\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto bus_stats = bus->get_statistics();
    display_statistics(bus_stats, stats);

    // ========================================
    // Cleanup
    // ========================================
    log->log(kcenon::thread::log_level::info, "[Cleanup] Shutting down components...");

    bus->shutdown();
    log->log(kcenon::thread::log_level::info, "[Message Bus] Shutdown complete");

    log->stop();
    std::cout << "[Logger] Stopped\n\n";

    // Summary
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Integration Summary\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "✓ messaging_system:       SUCCESS (" << stats.total_published.load()
              << " messages published)\n";
    std::cout << "✓ logger_system:          SUCCESS\n";
    std::cout << "✓ Pub/Sub Pattern:        " << stats.total_received.load()
              << " messages received\n";
    std::cout << "✓ Request/Reply Pattern:  " << stats.responses_received.load()
              << "/" << stats.requests_sent.load() << " responses\n";
    std::cout << "✓ Topics:                 " << bus->get_topics().size() << " active\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    std::cout << "[System] Messaging integration sample completed successfully.\n";

    return 0;
}
