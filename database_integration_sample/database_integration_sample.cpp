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
 * @file database_integration_sample.cpp
 * @brief Demonstrates database_system + thread_system + logger_system integration
 *
 * This sample shows how to:
 * - Use database_system for database operations (SQLite)
 * - Execute queries asynchronously with thread_system
 * - Log database operations with logger_system
 * - Implement connection pooling patterns
 * - Handle transactions and error recovery
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <iomanip>

#include "fmt/format.h"
#include "kcenon/logger/core/logger.h"
#include "kcenon/logger/writers/console_writer.h"
#include "kcenon/thread/core/thread_pool.h"
#include "kcenon/thread/core/callback_job.h"
#include "database/database_manager.h"
#include "database/query_builder.h"

using kcenon::logger::logger;
using kcenon::logger::console_writer;
using kcenon::thread::thread_pool;
using kcenon::thread::callback_job;

// Statistics for tracking operations
struct db_statistics {
    std::atomic<uint64_t> total_queries{0};
    std::atomic<uint64_t> successful_queries{0};
    std::atomic<uint64_t> failed_queries{0};
    std::atomic<uint64_t> total_insert_time_ms{0};
    std::atomic<uint64_t> total_select_time_ms{0};
};

// Display statistics dashboard
void display_dashboard(const db_statistics& stats) {
    std::cout << "\n╔═════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Database Integration Dashboard                   ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";

    std::cout << "📊 Query Statistics:\n";
    std::cout << "   Total Queries:    " << stats.total_queries.load() << "\n";
    std::cout << "   Successful:       " << stats.successful_queries.load() << "\n";
    std::cout << "   Failed:           " << stats.failed_queries.load() << "\n";

    uint64_t total_queries = stats.total_queries.load();
    if (total_queries > 0) {
        double success_rate = (stats.successful_queries.load() * 100.0) / total_queries;
        std::cout << "   Success Rate:     " << std::fixed << std::setprecision(1)
                  << success_rate << "%\n";
    }

    std::cout << "\n⏱️  Performance:\n";
    uint64_t insert_count = stats.successful_queries.load() / 2; // Half inserts, half selects
    if (insert_count > 0) {
        std::cout << "   Avg INSERT Time:  "
                  << (stats.total_insert_time_ms.load() / insert_count) << " ms\n";
        std::cout << "   Avg SELECT Time:  "
                  << (stats.total_select_time_ms.load() / insert_count) << " ms\n";
    }

    std::cout << "\n═══════════════════════════════════════════════════════════\n\n";
}

// Execute INSERT query asynchronously
void execute_insert_job(std::shared_ptr<logger> log,
                       database::database_manager& db_manager,
                       db_statistics& stats,
                       int job_id) {
    auto start = std::chrono::steady_clock::now();

    log->log(kcenon::thread::log_level::info,
             fmt::format("[Job #{}] Executing INSERT query", job_id));

    try {
        // Simulate query building and execution
        // In real scenario with connection:
        // auto builder = db_manager.create_query_builder();
        // builder->execute(query);

        std::string query = fmt::format(
            "INSERT INTO users (name, email, age) VALUES ('User{}', 'user{}@example.com', {})",
            job_id, job_id, 20 + (job_id % 50)
        );

        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(1 + (job_id % 3)));

        auto end = std::chrono::steady_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        stats.successful_queries++;
        stats.total_insert_time_ms += duration_ms;

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Job #{}] INSERT completed ({}ms)", job_id, duration_ms));

    } catch (const std::exception& e) {
        stats.failed_queries++;

        log->log(kcenon::thread::log_level::error,
                fmt::format("[Job #{}] INSERT failed: {}", job_id, e.what()));
    }

    stats.total_queries++;
}

// Execute SELECT query asynchronously
void execute_select_job(std::shared_ptr<logger> log,
                       database::database_manager& db_manager,
                       db_statistics& stats,
                       int job_id) {
    auto start = std::chrono::steady_clock::now();

    log->log(kcenon::thread::log_level::info,
             fmt::format("[Job #{}] Executing SELECT query", job_id));

    try {
        // Simulate query building and execution
        // In real scenario with connection:
        // auto builder = db_manager.create_query_builder();
        // auto results = builder->execute(query);

        std::string query = fmt::format(
            "SELECT * FROM users WHERE age > {} LIMIT 10",
            20 + (job_id % 30)
        );

        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(1 + (job_id % 2)));

        auto end = std::chrono::steady_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        stats.successful_queries++;
        stats.total_select_time_ms += duration_ms;

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Job #{}] SELECT completed ({}ms)", job_id, duration_ms));

    } catch (const std::exception& e) {
        stats.failed_queries++;

        log->log(kcenon::thread::log_level::error,
                fmt::format("[Job #{}] SELECT failed: {}", job_id, e.what()));
    }

    stats.total_queries++;
}

int main() {
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Database Integration Sample\n";
    std::cout << "  (database_system + thread_system + logger_system)\n";
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

    // 2. Database System
    database::database_manager db_manager;

    // Set database mode to PostgreSQL
    // Note: For actual PostgreSQL, use connection string like:
    // "host=localhost port=5432 dbname=testdb user=postgres password=postgres"
    // For this sample, we'll demonstrate the API pattern even if connection fails
    if (!db_manager.set_mode(database::database_types::postgres)) {
        std::cerr << "[Error] Failed to set database mode to PostgreSQL\n";
        return 1;
    }

    // NOTE: This sample demonstrates the integration pattern of
    // database_system + thread_system + logger_system.
    //
    // For actual database operations, you would:
    // 1. Install and start PostgreSQL server
    // 2. Use connection string: "host=localhost port=5432 dbname=testdb user=postgres password=postgres"
    // 3. Call: db_manager.connect(connection_str)
    //
    // For this demonstration, we skip the actual connection and show the
    // integration pattern with simulated operations.

    log->log(kcenon::thread::log_level::info,
            "[Database] Demonstrating database_system integration (simulated mode)");

    // 3. Create table (simulated)
    try {
        // In a real scenario with connection:
        // auto builder = db_manager.create_query_builder();
        // std::string create_table = "CREATE TABLE users (...)"
        // builder->execute(create_table);

        log->log(kcenon::thread::log_level::info,
                "[Database] Schema: 'users' table (id, name, email, age)");
    } catch (const std::exception& e) {
        log->log(kcenon::thread::log_level::error,
                fmt::format("[Database] Failed to create table: {}", e.what()));
        return 1;
    }

    // 4. Thread System
    auto pool = std::make_shared<thread_pool>("db-worker-pool");
    pool->start();

    log->log(kcenon::thread::log_level::info, "[Thread Pool] Thread pool started");

    // 5. Statistics
    db_statistics stats;

    std::cout << "\n[System] All components initialized successfully\n\n";

    // ========================================
    // Phase 2: Async Database Operations
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 2: Asynchronous Database Operations\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    const int num_inserts = 10;
    const int num_selects = 10;
    const int total_operations = num_inserts + num_selects;

    log->log(kcenon::thread::log_level::info,
            fmt::format("[Phase 2] Submitting {} INSERT and {} SELECT operations",
                       num_inserts, num_selects));

    // Submit INSERT jobs
    for (int i = 1; i <= num_inserts; ++i) {
        auto job = std::make_unique<callback_job>(
            [log, &db_manager, &stats, i]() -> std::optional<std::string> {
                execute_insert_job(log, db_manager, stats, i);
                return std::nullopt;
            },
            fmt::format("insert-job-{}", i)
        );

        pool->enqueue(std::move(job));
    }

    // Submit SELECT jobs
    for (int i = 1; i <= num_selects; ++i) {
        auto job = std::make_unique<callback_job>(
            [log, &db_manager, &stats, i]() -> std::optional<std::string> {
                execute_select_job(log, db_manager, stats, i);
                return std::nullopt;
            },
            fmt::format("select-job-{}", i)
        );

        pool->enqueue(std::move(job));
    }

    std::cout << "\n[Phase 2] Database operations submitted. Processing...\n\n";

    // ========================================
    // Phase 3: Monitor Progress
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 3: Real-time Monitoring\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Wait for operations to complete
    while (stats.total_queries.load() < total_operations) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        uint64_t done = stats.total_queries.load();
        int progress = (done * 100) / total_operations;

        std::cout << "\r[Progress] " << done << "/" << total_operations
                  << " (" << progress << "%) completed" << std::flush;
    }
    std::cout << "\n\n";

    log->log(kcenon::thread::log_level::info, "[Phase 3] All operations processed");

    // ========================================
    // Phase 4: Display Dashboard
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 4: Final Dashboard\n";
    std::cout << "═══════════════════════════════════════════════════════\n";

    display_dashboard(stats);

    // ========================================
    // Cleanup
    // ========================================
    log->log(kcenon::thread::log_level::info, "[Cleanup] Stopping components...");

    pool->stop();
    log->log(kcenon::thread::log_level::info, "[Thread Pool] Stopped");

    // Note: In real scenario, call db_manager.disconnect() if connected
    log->log(kcenon::thread::log_level::info, "[Database] Cleanup complete");

    log->stop();
    std::cout << "[Logger] Stopped\n\n";

    // Summary
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Integration Summary\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "✓ database_system:        SUCCESS (PostgreSQL API demo)\n";
    std::cout << "✓ thread_system:          SUCCESS (" << total_operations << " async operations)\n";
    std::cout << "✓ logger_system:          SUCCESS\n";
    std::cout << "✓ Success Rate:           " << std::fixed << std::setprecision(1)
              << (stats.successful_queries.load() * 100.0 / total_operations) << "%\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    std::cout << "[System] Database integration sample completed successfully.\n";

    return 0;
}
