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
 * @file combined_sample.cpp
 * @brief Demonstrates integration of Logger, Container, and Threads
 *
 * This sample shows how to:
 * - Use logger with thread pool for async operations
 * - Store processing results in containers
 * - Combine multiple systems for real-world scenarios
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

#include "fmt/format.h"
#include "kcenon/logger/core/logger.h"
#include "kcenon/logger/writers/console_writer.h"
#include "kcenon/thread/core/thread_pool.h"
#include "kcenon/thread/core/callback_job.h"
#include "container/container.h"

// Use explicit types to avoid namespace conflicts
using kcenon::logger::logger;
using kcenon::logger::console_writer;
using kcenon::thread::thread_pool;
using kcenon::thread::callback_job;
using container_module::value_container;
using container_module::value;
using container_module::value_types;

std::atomic<int> jobs_completed{0};

int main()
{
    std::cout << "=================================================\n";
    std::cout << "  Combined Integration Sample\n";
    std::cout << "  (Logger + Container + Threads)\n";
    std::cout << "=================================================\n" << std::endl;

    // Create logger
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>());
    log->start();

    log->log(kcenon::thread::log_level::info, "[Combined] Starting integrated example...");

    // Create thread pool
    auto pool = std::make_shared<thread_pool>("worker-pool");
    pool->start();

    log->log(kcenon::thread::log_level::info, "[Combined] Thread pool created with workers\n");

    // Create container to store results
    value_container results;

    // Submit jobs that use both logger and container
    for (int i = 0; i < 10; ++i) {
        auto job = std::make_unique<callback_job>(
            [i, log]() -> std::optional<std::string> {
                // Log the processing
                log->log(kcenon::thread::log_level::info,
                    fmt::format("[Worker] Processing job {}", i));

                // Simulate some work
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Create result container
                value_container result;
                result.add(std::make_shared<value>("job_id", value_types::int_value,
                    std::to_string(i)));
                result.add(std::make_shared<value>("status", value_types::string_value,
                    "completed"));
                result.add(std::make_shared<value>("result", value_types::int_value,
                    std::to_string(i * 10)));

                // Log completion
                log->log(kcenon::thread::log_level::info,
                    fmt::format("[Worker] Job {} completed with result: {}",
                        i, i * 10));

                jobs_completed++;
                return std::nullopt; // Success
            },
            fmt::format("job-{}", i)
        );

        pool->enqueue(std::move(job));
    }

    // Wait for all jobs to complete
    log->log(kcenon::thread::log_level::info, "\n[Combined] Waiting for jobs to complete...");
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Display statistics
    std::cout << "\n=================================================\n";
    std::cout << "  Results\n";
    std::cout << "=================================================\n";
    std::cout << fmt::format("Jobs Completed: {}\n", jobs_completed.load());
    std::cout << "=================================================\n" << std::endl;

    // Cleanup
    log->log(kcenon::thread::log_level::info, "[Combined] Shutting down...");
    pool->stop();
    log->stop();

    std::cout << "[Combined] Complete\n" << std::endl;

    return 0;
}
