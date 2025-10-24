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
 * @file monitoring_sample.cpp
 * @brief Demonstrates performance monitoring and resource tracking
 *
 * This sample shows how to:
 * - Track code execution time with high-precision timers
 * - Monitor system resources (memory usage on macOS/Linux)
 * - Collect and analyze performance metrics
 * - Display real-time monitoring dashboard
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <vector>
#include <random>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <cmath>

#ifdef __APPLE__
    #include <mach/mach.h>
    #include <sys/sysctl.h>
#elif __linux__
    #include <sys/sysinfo.h>
    #include <unistd.h>
#endif

// Simple performance profiler implementation
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
        std::chrono::nanoseconds median_duration{std::chrono::nanoseconds::zero()};
        double throughput{0.0};
    };

    void record_sample(const std::string& operation_name,
                      std::chrono::nanoseconds duration,
                      bool success = true) {
        auto& metric = metrics_[operation_name];
        metric.samples.push_back(duration);
        metric.call_count++;
        if (!success) {
            metric.error_count++;
        }
    }

    performance_summary get_summary(const std::string& operation_name) const {
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
        summary.median_duration = sorted[sorted.size() / 2];

        // Calculate throughput (ops/sec)
        double total_seconds = std::chrono::duration<double>(total).count();
        summary.throughput = sorted.size() / total_seconds;

        return summary;
    }

    std::vector<performance_summary> get_all_summaries() const {
        std::vector<performance_summary> summaries;
        for (const auto& [name, _] : metrics_) {
            summaries.push_back(get_summary(name));
        }
        return summaries;
    }

private:
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

// System resource information
struct system_resources {
    size_t total_memory_bytes{0};
    size_t used_memory_bytes{0};
    size_t available_memory_bytes{0};
    double memory_usage_percent{0.0};
    int cpu_count{0};
};

// Helper function to get system resources
system_resources get_system_resources() {
    system_resources resources;

#ifdef __APPLE__
    // Get CPU count
    int mib[2] = {CTL_HW, HW_NCPU};
    size_t len = sizeof(resources.cpu_count);
    sysctl(mib, 2, &resources.cpu_count, &len, nullptr, 0);

    // Get memory info
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vm_stat;
    host_statistics64(mach_host_self(), HOST_VM_INFO64,
                     (host_info64_t)&vm_stat, &count);

    int64_t page_size;
    size_t size = sizeof(page_size);
    sysctlbyname("hw.pagesize", &page_size, &size, nullptr, 0);

    int64_t total_mem;
    size = sizeof(total_mem);
    sysctlbyname("hw.memsize", &total_mem, &size, nullptr, 0);

    resources.total_memory_bytes = total_mem;
    resources.available_memory_bytes = vm_stat.free_count * page_size;
    resources.used_memory_bytes = resources.total_memory_bytes - resources.available_memory_bytes;
    resources.memory_usage_percent = (resources.used_memory_bytes * 100.0) / resources.total_memory_bytes;

#elif __linux__
    struct sysinfo si;
    sysinfo(&si);

    resources.cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    resources.total_memory_bytes = si.totalram * si.mem_unit;
    resources.available_memory_bytes = si.freeram * si.mem_unit;
    resources.used_memory_bytes = resources.total_memory_bytes - resources.available_memory_bytes;
    resources.memory_usage_percent = (resources.used_memory_bytes * 100.0) / resources.total_memory_bytes;
#else
    resources.cpu_count = std::thread::hardware_concurrency();
#endif

    return resources;
}

// Helper function to format bytes
std::string format_bytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return ss.str();
}

// Helper function to format duration
std::string format_duration(std::chrono::nanoseconds duration) {
    auto ns = duration.count();
    if (ns < 1000) return std::to_string(ns) + " ns";
    if (ns < 1000000) return std::to_string(ns / 1000) + " μs";
    if (ns < 1000000000) return std::to_string(ns / 1000000) + " ms";
    return std::to_string(ns / 1000000000) + " s";
}

// Display system dashboard
void display_dashboard(const system_resources& resources) {
    std::cout << "\n┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│           System Resource Dashboard                    │\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n\n";

    std::cout << "🖥️  CPU Cores:        " << resources.cpu_count << "\n\n";

    std::cout << "💾 Memory Metrics:\n";
    std::cout << "   Total Memory:     " << format_bytes(resources.total_memory_bytes) << "\n";
    std::cout << "   Used Memory:      " << format_bytes(resources.used_memory_bytes) << "\n";
    std::cout << "   Available Memory: " << format_bytes(resources.available_memory_bytes) << "\n";
    std::cout << "   Memory Usage:     " << std::fixed << std::setprecision(1)
              << resources.memory_usage_percent << "%\n\n";
}

// Display performance metrics
void display_performance_metrics(const std::vector<performance_profiler::performance_summary>& summaries) {
    std::cout << "\n┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│           Performance Metrics Summary                  │\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n\n";

    for (const auto& summary : summaries) {
        std::cout << "📊 Operation: " << summary.operation_name << "\n";
        std::cout << "   Calls:       " << summary.call_count << "\n";
        std::cout << "   Errors:      " << summary.error_count << "\n";
        std::cout << "   Throughput:  " << std::fixed << std::setprecision(2)
                  << summary.throughput << " ops/sec\n";
        std::cout << "   Min:         " << format_duration(summary.min_duration) << "\n";
        std::cout << "   Mean:        " << format_duration(summary.mean_duration) << "\n";
        std::cout << "   Median:      " << format_duration(summary.median_duration) << "\n";
        std::cout << "   Max:         " << format_duration(summary.max_duration) << "\n\n";
    }
}

// Simulated CPU-intensive work
void cpu_intensive_work(performance_profiler& profiler, int workload_ms) {
    scoped_timer timer(profiler, "cpu_intensive_work");

    auto start = std::chrono::steady_clock::now();
    volatile double result = 0.0;
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(workload_ms)) {
        for (int i = 0; i < 10000; ++i) {
            result += std::sin(i) * std::cos(i);
        }
    }
}

// Simulated memory-intensive work
void memory_intensive_work(performance_profiler& profiler, size_t allocation_mb) {
    scoped_timer timer(profiler, "memory_intensive_work");

    std::vector<std::vector<uint8_t>> buffers;
    for (size_t i = 0; i < allocation_mb; ++i) {
        buffers.emplace_back(1024 * 1024, static_cast<uint8_t>(i % 256));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main() {
    std::cout << "=================================================\n";
    std::cout << "  Performance Monitoring Sample\n";
    std::cout << "=================================================\n\n";

    performance_profiler profiler;

    std::cout << "[Monitoring] Initializing performance monitoring...\n\n";

    // Collect baseline system resources
    std::cout << "[Phase 1] Collecting baseline system resources...\n";
    auto baseline_resources = get_system_resources();
    display_dashboard(baseline_resources);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Run CPU-intensive workload
    std::cout << "[Phase 2] Running CPU-intensive workload...\n";
    std::cout << "[Workload] Executing 10 iterations with varying intensity...\n\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> workload_dist(10, 50);

    for (int i = 0; i < 10; ++i) {
        int workload_ms = workload_dist(gen);
        std::cout << "[CPU Work " << (i + 1) << "] Intensity: " << workload_ms << "ms\n";
        cpu_intensive_work(profiler, workload_ms);
    }

    auto cpu_resources = get_system_resources();
    display_dashboard(cpu_resources);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Run memory-intensive workload
    std::cout << "[Phase 3] Running memory-intensive workload...\n";
    std::cout << "[Workload] Allocating memory buffers...\n\n";

    for (int i = 0; i < 5; ++i) {
        size_t allocation_mb = (i + 1) * 10;
        std::cout << "[Memory Work " << (i + 1) << "] Allocating: " << allocation_mb << " MB\n";
        memory_intensive_work(profiler, allocation_mb);
    }

    auto memory_resources = get_system_resources();
    display_dashboard(memory_resources);

    // Display performance summary
    std::cout << "[Phase 4] Analyzing performance metrics...\n";
    auto all_summaries = profiler.get_all_summaries();
    display_performance_metrics(all_summaries);

    // Final system state
    std::cout << "[Phase 5] Final system resource snapshot...\n";
    auto final_resources = get_system_resources();
    display_dashboard(final_resources);

    // Summary
    std::cout << "=================================================\n";
    std::cout << "  Monitoring Summary\n";
    std::cout << "=================================================\n";
    std::cout << "Memory Usage Change: " << std::fixed << std::setprecision(1)
              << (final_resources.memory_usage_percent - baseline_resources.memory_usage_percent) << "%\n";
    std::cout << "Operations Profiled: " << all_summaries.size() << "\n";

    size_t total_calls = 0;
    for (const auto& summary : all_summaries) {
        total_calls += summary.call_count;
    }
    std::cout << "Total Profile Calls: " << total_calls << "\n";
    std::cout << "=================================================\n\n";

    std::cout << "[Monitoring] Sample completed successfully.\n";

    return 0;
}
