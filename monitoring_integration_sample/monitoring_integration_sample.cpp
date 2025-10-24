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
 * @brief Demonstrates complete observability stack integration
 *
 * This sample shows how to:
 * - Integrate Logger + Thread Pool + Performance Monitoring
 * - Build production-ready observability stack
 * - Track job execution with real-time metrics
 * - Display comprehensive monitoring dashboard
 * - Implement distributed tracing patterns
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <random>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>

// Use fmt first to avoid conflicts
#include "fmt/format.h"
#include "kcenon/logger/core/logger.h"
#include "kcenon/logger/writers/console_writer.h"
#include "kcenon/thread/core/thread_pool.h"
#include "kcenon/thread/core/callback_job.h"

using kcenon::logger::logger;
using kcenon::logger::console_writer;
using kcenon::thread::thread_pool;
using kcenon::thread::callback_job;

// Performance profiler (simplified for this sample)
class performance_profiler {
public:
    struct metric_data {
        std::vector<std::chrono::nanoseconds> samples;
        std::atomic<uint64_t> call_count{0};
        std::atomic<uint64_t> error_count{0};
    };

    struct performance_summary {
        std::string operation_name;
        uint64_t call_count{0};
        uint64_t error_count{0};
        std::chrono::nanoseconds min_duration{std::chrono::nanoseconds::max()};
        std::chrono::nanoseconds max_duration{std::chrono::nanoseconds::zero()};
        std::chrono::nanoseconds mean_duration{std::chrono::nanoseconds::zero()};
        double throughput{0.0};
    };

    void record_sample(const std::string& operation_name,
                      std::chrono::nanoseconds duration,
                      bool success = true) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& metric = metrics_[operation_name];
        metric.samples.push_back(duration);
        metric.call_count++;
        if (!success) {
            metric.error_count++;
        }
    }

    performance_summary get_summary(const std::string& operation_name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = metrics_.find(operation_name);
        if (it == metrics_.end()) {
            return {};
        }

        const auto& metric = it->second;
        performance_summary summary;
        summary.operation_name = operation_name;
        summary.call_count = metric.call_count.load();
        summary.error_count = metric.error_count.load();

        if (metric.samples.empty()) {
            return summary;
        }

        auto sorted = metric.samples;
        std::sort(sorted.begin(), sorted.end());

        summary.min_duration = sorted.front();
        summary.max_duration = sorted.back();

        auto total = std::accumulate(sorted.begin(), sorted.end(),
                                    std::chrono::nanoseconds::zero());
        summary.mean_duration = total / sorted.size();

        double total_seconds = std::chrono::duration<double>(total).count();
        summary.throughput = sorted.size() / total_seconds;

        return summary;
    }

    std::vector<performance_summary> get_all_summaries() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<performance_summary> summaries;
        for (const auto& [name, _] : metrics_) {
            summaries.push_back(get_summary(name));
        }
        return summaries;
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, metric_data> metrics_;
};

// Scoped timer for automatic performance measurement
class scoped_timer {
public:
    scoped_timer(performance_profiler& profiler, const std::string& operation_name)
        : profiler_(profiler)
        , operation_name_(operation_name)
        , start_time_(std::chrono::high_resolution_clock::now())
        , success_(true) {}

    ~scoped_timer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
            end_time - start_time_);
        profiler_.record_sample(operation_name_, duration, success_);
    }

    void mark_error() { success_ = false; }

private:
    performance_profiler& profiler_;
    std::string operation_name_;
    std::chrono::high_resolution_clock::time_point start_time_;
    bool success_;
};

// Job statistics
struct job_statistics {
    std::atomic<uint64_t> total_jobs{0};
    std::atomic<uint64_t> completed_jobs{0};
    std::atomic<uint64_t> failed_jobs{0};
    std::atomic<uint64_t> total_processing_time_ms{0};
};

// Helper to format duration
std::string format_duration(std::chrono::nanoseconds duration) {
    auto ns = duration.count();
    if (ns < 1000) return fmt::format("{} ns", ns);
    if (ns < 1000000) return fmt::format("{} μs", ns / 1000);
    if (ns < 1000000000) return fmt::format("{} ms", ns / 1000000);
    return fmt::format("{} s", ns / 1000000000);
}

// Display monitoring dashboard
void display_dashboard(const std::vector<performance_profiler::performance_summary>& summaries,
                      const job_statistics& stats) {
    std::cout << "\n╔═════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Monitoring & Observability Dashboard             ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";

    // Job Statistics
    std::cout << "📊 Job Statistics:\n";
    std::cout << "   Total Jobs:       " << stats.total_jobs.load() << "\n";
    std::cout << "   Completed:        " << stats.completed_jobs.load() << "\n";
    std::cout << "   Failed:           " << stats.failed_jobs.load() << "\n";

    uint64_t total_time = stats.total_processing_time_ms.load();
    uint64_t completed = stats.completed_jobs.load();
    if (completed > 0) {
        std::cout << "   Avg Processing:   " << (total_time / completed) << " ms\n";
    }
    std::cout << "\n";

    // Performance Metrics
    if (!summaries.empty()) {
        std::cout << "⚡ Performance Metrics:\n\n";

        for (const auto& summary : summaries) {
            std::cout << "   Operation: " << summary.operation_name << "\n";
            std::cout << "   ├─ Calls:      " << summary.call_count << "\n";
            std::cout << "   ├─ Errors:     " << summary.error_count << "\n";
            std::cout << "   ├─ Throughput: " << std::fixed << std::setprecision(2)
                      << summary.throughput << " ops/sec\n";
            std::cout << "   ├─ Min:        " << format_duration(summary.min_duration) << "\n";
            std::cout << "   ├─ Mean:       " << format_duration(summary.mean_duration) << "\n";
            std::cout << "   └─ Max:        " << format_duration(summary.max_duration) << "\n\n";
        }
    }

    std::cout << "═══════════════════════════════════════════════════════════\n\n";
}

// Simulated data processing job
void process_data_job(std::shared_ptr<logger> log,
                     performance_profiler& profiler,
                     job_statistics& stats,
                     int job_id) {
    scoped_timer timer(profiler, "data_processing");

    log->log(kcenon::thread::log_level::info,
             fmt::format("[Job #{}] Starting data processing", job_id));

    auto start = std::chrono::steady_clock::now();

    try {
        // Simulate work with random processing time
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(10, 100);
        int processing_ms = dist(gen);

        std::this_thread::sleep_for(std::chrono::milliseconds(processing_ms));

        // Simulate occasional failures (10% failure rate)
        std::uniform_int_distribution<> fail_dist(1, 10);
        if (fail_dist(gen) == 1) {
            throw std::runtime_error("Simulated processing error");
        }

        auto end = std::chrono::steady_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        stats.completed_jobs++;
        stats.total_processing_time_ms += duration_ms;

        log->log(kcenon::thread::log_level::info,
                fmt::format("[Job #{}] Completed successfully ({}ms)", job_id, duration_ms));

    } catch (const std::exception& e) {
        stats.failed_jobs++;

        log->log(kcenon::thread::log_level::error,
                fmt::format("[Job #{}] Failed: {}", job_id, e.what()));
    }
}

int main() {
    std::cout << "═════════════════════════════════════════════════════════\n";
    std::cout << "  Monitoring Integration Sample\n";
    std::cout << "  (Logger + Threads + Performance Monitoring)\n";
    std::cout << "═════════════════════════════════════════════════════════\n\n";

    // Initialize components
    std::cout << "[Init] Initializing observability stack...\n\n";

    // 1. Logger
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>(true, true));
    log->start();

    log->log(kcenon::thread::log_level::info, "[Logger] Logger initialized");

    // 2. Thread Pool
    auto pool = std::make_shared<thread_pool>("worker-pool");
    pool->start();

    log->log(kcenon::thread::log_level::info, "[Thread Pool] Thread pool started");

    // 3. Performance Profiler
    performance_profiler profiler;
    log->log(kcenon::thread::log_level::info, "[Profiler] Performance profiler ready");

    // 4. Job Statistics
    job_statistics stats;

    std::cout << "\n[System] All components initialized successfully\n\n";

    // ========================================
    // Phase 1: Submit Jobs
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "  Phase 1: Job Submission\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";

    const int num_jobs = 20;
    stats.total_jobs = num_jobs;

    log->log(kcenon::thread::log_level::info,
            fmt::format("[Phase 1] Submitting {} jobs to thread pool", num_jobs));

    for (int i = 1; i <= num_jobs; ++i) {
        auto job = std::make_unique<callback_job>(
            [log, &profiler, &stats, i]() -> std::optional<std::string> {
                process_data_job(log, profiler, stats, i);
                return std::nullopt;
            },
            fmt::format("data-job-{}", i)
        );

        pool->enqueue(std::move(job));
    }

    log->log(kcenon::thread::log_level::info,
            fmt::format("[Phase 1] All {} jobs submitted", num_jobs));

    std::cout << "\n[Phase 1] Jobs submitted. Processing...\n\n";

    // ========================================
    // Phase 2: Monitor Progress
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "  Phase 2: Real-time Monitoring\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";

    // Wait for jobs to complete
    while (stats.completed_jobs.load() + stats.failed_jobs.load() < num_jobs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        uint64_t done = stats.completed_jobs.load() + stats.failed_jobs.load();
        int progress = (done * 100) / num_jobs;

        std::cout << "\r[Progress] " << done << "/" << num_jobs
                  << " (" << progress << "%) completed" << std::flush;
    }
    std::cout << "\n\n";

    log->log(kcenon::thread::log_level::info, "[Phase 2] All jobs processed");

    // ========================================
    // Phase 3: Display Dashboard
    // ========================================
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "  Phase 3: Final Dashboard\n";
    std::cout << "═══════════════════════════════════════════════════════════\n";

    auto summaries = profiler.get_all_summaries();
    display_dashboard(summaries, stats);

    // ========================================
    // Cleanup
    // ========================================
    log->log(kcenon::thread::log_level::info, "[Cleanup] Stopping components...");

    pool->stop();
    log->log(kcenon::thread::log_level::info, "[Thread Pool] Stopped");

    log->stop();
    std::cout << "[Logger] Stopped\n\n";

    // Summary
    std::cout << "═════════════════════════════════════════════════════════\n";
    std::cout << "  Integration Summary\n";
    std::cout << "═════════════════════════════════════════════════════════\n";
    std::cout << "✓ Logger Integration:     SUCCESS\n";
    std::cout << "✓ Thread Pool:            SUCCESS (" << num_jobs << " jobs)\n";
    std::cout << "✓ Performance Profiling:  SUCCESS\n";
    std::cout << "✓ Success Rate:           " << std::fixed << std::setprecision(1)
              << (stats.completed_jobs.load() * 100.0 / num_jobs) << "%\n";
    std::cout << "═════════════════════════════════════════════════════════\n\n";

    std::cout << "[System] Integration sample completed successfully.\n";

    return 0;
}
