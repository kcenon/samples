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
 * @file monitoring_integration_sample.cpp
 * @brief Demonstrates monitoring_system + thread_system + logger_system integration
 *
 * This sample shows how to:
 * - Monitor system resources (CPU, memory, threads)
 * - Profile performance of operations
 * - Track thread pool job execution
 * - Integrate monitoring with logger_system
 * - Display real-time metrics dashboard
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
#include "kcenon/monitoring/core/performance_monitor.h"

using kcenon::logger::logger;
using kcenon::logger::console_writer;
using kcenon::thread::thread_pool;
using kcenon::thread::callback_job;
using monitoring_system::performance_profiler;
using monitoring_system::performance_metrics;
using monitoring_system::system_monitor;
using monitoring_system::system_metrics;

// Statistics for tracking
struct monitoring_statistics {
    std::atomic<uint64_t> total_jobs{0};
    std::atomic<uint64_t> completed_jobs{0};
    std::atomic<uint64_t> monitored_operations{0};
    std::vector<system_metrics> resource_snapshots;
    std::mutex snapshots_mutex;
};

// Display system resources
void display_system_resources(const system_metrics& resources) {
    std::cout << "\n╔═════════════════════════════════════════════════════════╗\n";
    std::cout << "║       System Resources Dashboard                      ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";

    std::cout << "💻 CPU Metrics:\n";
    std::cout << "   CPU Usage:        " << std::fixed << std::setprecision(1)
              << resources.cpu_usage_percent << "%\n";

    std::cout << "\n💾 Memory Metrics:\n";
    std::cout << "   Used Memory:      "
              << (resources.memory_usage_bytes / (1024 * 1024)) << " MB\n";
    std::cout << "   Available Memory: "
              << (resources.available_memory_bytes / (1024 * 1024)) << " MB\n";
    std::cout << "   Memory Usage:     " << std::fixed << std::setprecision(1)
              << resources.memory_usage_percent << "%\n";

    std::cout << "\n🧵 Process Metrics:\n";
    std::cout << "   Thread Count:     " << resources.thread_count << "\n";
    std::cout << "   Handle Count:     " << resources.handle_count << "\n";

    std::cout << "\n📊 I/O Metrics:\n";
    std::cout << "   Disk Read:        " << std::fixed << std::setprecision(2)
              << resources.disk_io_read_rate << " MB/s\n";
    std::cout << "   Disk Write:       " << std::fixed << std::setprecision(2)
              << resources.disk_io_write_rate << " MB/s\n";
    std::cout << "   Network Recv:     " << std::fixed << std::setprecision(2)
              << resources.network_io_recv_rate << " MB/s\n";
    std::cout << "   Network Send:     " << std::fixed << std::setprecision(2)
              << resources.network_io_send_rate << " MB/s\n";

    std::cout << "\n═══════════════════════════════════════════════════════════\n\n";
}

// Display performance metrics
void display_performance_metrics(const std::vector<performance_metrics>& metrics) {
    std::cout << "\n╔═════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Performance Metrics Dashboard                   ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";

    for (const auto& metric : metrics) {
        std::cout << "📊 Operation: " << metric.operation_name << "\n";
        std::cout << "   Call Count:       " << metric.call_count << "\n";
        std::cout << "   Error Count:      " << metric.error_count << "\n";

        if (metric.call_count > 0) {
            auto to_ms = [](std::chrono::nanoseconds ns) {
                return std::chrono::duration_cast<std::chrono::microseconds>(ns).count() / 1000.0;
            };

            std::cout << "   Min Duration:     " << std::fixed << std::setprecision(2)
                      << to_ms(metric.min_duration) << " ms\n";
            std::cout << "   Max Duration:     " << std::fixed << std::setprecision(2)
                      << to_ms(metric.max_duration) << " ms\n";
            std::cout << "   Mean Duration:    " << std::fixed << std::setprecision(2)
                      << to_ms(metric.mean_duration) << " ms\n";
            std::cout << "   Median Duration:  " << std::fixed << std::setprecision(2)
                      << to_ms(metric.median_duration) << " ms\n";
            std::cout << "   P95 Duration:     " << std::fixed << std::setprecision(2)
                      << to_ms(metric.p95_duration) << " ms\n";
            std::cout << "   P99 Duration:     " << std::fixed << std::setprecision(2)
                      << to_ms(metric.p99_duration) << " ms\n";
            std::cout << "   Throughput:       " << std::fixed << std::setprecision(2)
                      << metric.throughput << " ops/sec\n";
        }
        std::cout << "\n";
    }

    std::cout << "═══════════════════════════════════════════════════════════\n\n";
}

// Simulate compute-intensive work
void compute_task(int iterations) {
    volatile uint64_t result = 0;
    for (int i = 0; i < iterations * 10000; ++i) {
        result += i * i;
    }
}

// Execute monitored job
void execute_monitored_job(std::shared_ptr<logger> log,
                          performance_profiler& profiler,
                          monitoring_statistics& stats,
                          int job_id,
                          int complexity) {
    log->log(kcenon::thread::log_level::info,
             fmt::format("[Job #{}] Starting (complexity: {})", job_id, complexity));

    // Profile the operation
    auto operation_name = fmt::format("compute_job_{}", job_id % 3);

    auto start = std::chrono::high_resolution_clock::now();

    try {
        // Perform work
        compute_task(complexity);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        // Record performance sample
        profiler.record_sample(operation_name, duration);

        stats.completed_jobs++;
        stats.monitored_operations++;

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Job #{}] Completed", job_id));

    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        profiler.record_sample(operation_name, duration, false);

        log->log(kcenon::thread::log_level::error,
                fmt::format("[Job #{}] Failed: {}", job_id, e.what()));
    }

    stats.total_jobs++;
}

int main() {
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Monitoring Integration Sample\n";
    std::cout << "  (monitoring + thread + logger systems)\n";
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

    // 2. Monitoring System Components
    performance_profiler profiler;
    system_monitor resource_monitor;
    monitoring_statistics stats;

    log->log(kcenon::thread::log_level::info, "[Monitoring] Performance profiler initialized");
    log->log(kcenon::thread::log_level::info, "[Monitoring] System monitor initialized");

    // 3. Thread Pool
    auto pool = std::make_shared<thread_pool>("monitored-pool");
    pool->start();

    log->log(kcenon::thread::log_level::info, "[Thread Pool] Thread pool started");

    std::cout << "\n[System] All components initialized successfully\n\n";

    // ========================================
    // Phase 2: Collect Baseline System Resources
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 2: Baseline System Resources\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    auto baseline_result = resource_monitor.get_current_metrics();
    if (baseline_result) {
        auto baseline_resources = baseline_result.value();
        {
            std::lock_guard<std::mutex> lock(stats.snapshots_mutex);
            stats.resource_snapshots.push_back(baseline_resources);
        }
        display_system_resources(baseline_resources);
    } else {
        log->log(kcenon::thread::log_level::warning,
                "[Phase 2] Failed to collect baseline metrics");
    }

    // ========================================
    // Phase 3: Execute Monitored Jobs
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 3: Execute Monitored Jobs\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    const int num_jobs = 15;
    log->log(kcenon::thread::log_level::info,
            fmt::format("[Phase 3] Submitting {} monitored jobs", num_jobs));

    // Submit jobs with varying complexity
    for (int i = 1; i <= num_jobs; ++i) {
        int complexity = 5 + (i % 3) * 5; // 5, 10, or 15

        auto job = std::make_unique<callback_job>(
            [log, &profiler, &stats, i, complexity]() -> std::optional<std::string> {
                execute_monitored_job(log, profiler, stats, i, complexity);
                return std::nullopt;
            },
            fmt::format("monitored-job-{}", i)
        );

        pool->enqueue(std::move(job));
    }

    std::cout << "\n[Phase 3] Jobs submitted. Processing...\n\n";

    // ========================================
    // Phase 4: Monitor Progress
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 4: Real-time Monitoring\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Monitor while jobs are running
    int snapshot_count = 0;
    while (stats.total_jobs.load() < num_jobs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        uint64_t done = stats.total_jobs.load();
        int progress = (done * 100) / num_jobs;

        std::cout << "\r[Progress] " << done << "/" << num_jobs
                  << " (" << progress << "%) completed" << std::flush;

        // Collect resource snapshot every 2 iterations
        if (snapshot_count % 4 == 0) {
            auto result = resource_monitor.get_current_metrics();
            if (result) {
                std::lock_guard<std::mutex> lock(stats.snapshots_mutex);
                stats.resource_snapshots.push_back(result.value());
            }
        }
        snapshot_count++;
    }
    std::cout << "\n\n";

    log->log(kcenon::thread::log_level::info, "[Phase 4] All jobs processed");

    // ========================================
    // Phase 5: Collect Final Metrics
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 5: Final System State\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto final_result = resource_monitor.get_current_metrics();
    if (final_result) {
        auto final_resources = final_result.value();
        {
            std::lock_guard<std::mutex> lock(stats.snapshots_mutex);
            stats.resource_snapshots.push_back(final_resources);
        }
        display_system_resources(final_resources);
    } else {
        log->log(kcenon::thread::log_level::warning,
                "[Phase 5] Failed to collect final metrics");
    }

    // ========================================
    // Phase 6: Display Performance Metrics
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Phase 6: Performance Metrics\n";
    std::cout << "═══════════════════════════════════════════════════════\n";

    auto metrics = profiler.get_all_metrics();
    display_performance_metrics(metrics);

    // ========================================
    // Cleanup
    // ========================================
    log->log(kcenon::thread::log_level::info, "[Cleanup] Stopping components...");

    pool->stop();
    log->log(kcenon::thread::log_level::info, "[Thread Pool] Stopped");

    log->stop();
    std::cout << "[Logger] Stopped\n\n";

    // Summary
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  Integration Summary\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "✓ monitoring_system:      SUCCESS (" << stats.monitored_operations.load() << " operations profiled)\n";
    std::cout << "✓ thread_system:          SUCCESS (" << num_jobs << " jobs processed)\n";
    std::cout << "✓ logger_system:          SUCCESS\n";
    std::cout << "✓ Resource Snapshots:     " << stats.resource_snapshots.size() << " collected\n";
    std::cout << "✓ Success Rate:           " << std::fixed << std::setprecision(1)
              << (stats.completed_jobs.load() * 100.0 / num_jobs) << "%\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    std::cout << "[System] Monitoring integration sample completed successfully.\n";

    return 0;
}
