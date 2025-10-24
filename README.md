# 🚀 System Samples Collection

<div align="center">

[![CodeFactor](https://www.codefactor.io/repository/github/kcenon/samples/badge)](https://www.codefactor.io/repository/github/kcenon/samples)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/7e2a1c9fcaf444c8938257687c1ee68a)](https://www.codacy.com/gh/kcenon/samples/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kcenon/samples&amp;utm_campaign=Badge_Grade)

**Practical examples demonstrating the C++ ecosystem components**

[📚 Samples](#-sample-programs) | [🔗 Quick Start](#-quick-start) | [📖 Examples](#-detailed-examples) | [🏗️ Architecture](#-ecosystem-integration)

</div>

---

## 🔗 Ecosystem Integration

This samples repository demonstrates practical usage of the modular C++ ecosystem components, showing how to integrate and use each system in real-world scenarios.

### Component Coverage

```
┌─────────────────────────────────────────────────┐
│           samples (This Repository)             │
│        Practical Usage Demonstrations           │
└──────────┬──────────────────────────────────────┘
           │
    ┌──────┴──────┬───────────┬────────────┬──────────┐
    │             │           │            │          │
    ▼             ▼           ▼            ▼          ▼
┌─────────┐  ┌─────────┐ ┌──────────┐ ┌─────────┐ ┌──────────┐
│ logger  │  │container│ │  thread  │ │ network │ │messaging │
│ sample  │  │ sample  │ │  sample  │ │ samples │ │  system  │
└─────────┘  └─────────┘ └──────────┘ └─────────┘ └──────────┘
     │            │            │           │            │
     └────────────┴────────────┴───────────┴────────────┘
                              │
                   ┌──────────▼────────────┐
                   │    Ecosystem Base     │
                   │  (common, thread,     │
                   │   logger, container,  │
                   │   network, messaging) │
                   └───────────────────────┘
```

### Sample Programs Overview

| Sample | Component | Lines of Code | Complexity | Use Case |
|--------|-----------|---------------|------------|----------|
| **[logging_sample](#1-logging_sample)** | logger_system | ~100 | ⭐ Easy | Async logging basics |
| **[container_sample](#2-container_sample)** | container_system | ~150 | ⭐⭐ Medium | Type-safe data containers |
| **[threads_sample](#3-threads_sample)** | thread_system | ~200 | ⭐⭐⭐ Advanced | Priority thread pools |
| **[echo_server](#4-echo_server)** | network_system (TCP) | ~180 | ⭐⭐ Medium | TCP server implementation |
| **[echo_client](#5-echo_client)** | network_system (TCP) | ~150 | ⭐⭐ Medium | TCP client implementation |
| **[udp_echo_server](#6-udp_echo_server)** | network_system (UDP) | ~165 | ⭐⭐ Medium | UDP server implementation |
| **[udp_echo_client](#7-udp_echo_client)** | network_system (UDP) | ~190 | ⭐⭐ Medium | UDP client implementation |
| **[combined_sample](#8-combined_sample)** | Multi-system | ~145 | ⭐⭐⭐ Advanced | Logger + Container + Threads integration |
| **[database_integration_sample](#9-database_integration_sample)** | database + thread + logger | ~350 | ⭐⭐⭐ Advanced | Async database operations with thread pool |
| **[thread_integration_sample](#10-thread_integration_sample)** | thread + logger | ~450 | ⭐⭐⭐ Advanced | Priority-based scheduling and performance tracking |
| **[monitoring_integration_sample](#11-monitoring_integration_sample)** | monitoring + thread + logger | ~400 | ⭐⭐⭐ Advanced | Real-time system monitoring and performance profiling |
| **[messaging_integration_sample](#12-messaging_integration_sample)** | messaging + logger | ~500 | ⭐⭐⭐⭐ Expert | Pub/Sub and Request/Reply messaging patterns |

---

## 🚀 Quick Start

### Step 1: Install (30 seconds)

```bash
# Clone repository
git clone https://github.com/kcenon/samples.git
cd samples

# Build all samples (dependencies are automatically fetched)
./build.sh --clean
```

**💡 Dependency Modes**: This project supports three modes for obtaining system dependencies:
- **Mode 1 (Default)**: FetchContent - Automatically downloads from GitHub
- **Mode 2**: Local systems - Uses local repositories
  ```bash
  cmake -B build -DSAMPLES_USE_LOCAL_SYSTEMS=ON
  ```
- **Mode 3**: Installed packages - Uses system-installed libraries
  ```bash
  cmake -B build -DSAMPLES_USE_INSTALLED_SYSTEMS=ON
  ```

### Step 2: Run Your First Sample (30 seconds)

```bash
# Run logging sample
./bin/logging_sample

# Expected output:
# [INFO] Application started
# [DEBUG] Processing data...
# [ERROR] Example error handling
```

### Step 3: Explore More Examples

```bash
# Container sample - Type-safe data handling
./bin/container_sample

# Thread pool sample - Concurrent processing
./bin/threads_sample

# Network samples - TCP Client/Server communication
./bin/echo_server &    # Start TCP server in background
./bin/echo_client      # Run TCP client

# Network samples - UDP Client/Server communication
./bin/udp_echo_server &  # Start UDP server in background
./bin/udp_echo_client    # Run UDP client

# Combined integration sample - Multi-system usage
./bin/combined_sample

# Database integration sample - Async database operations
./bin/database_integration_sample

# Thread integration sample - Priority scheduling and performance tracking
./bin/thread_integration_sample

# Monitoring integration sample - Real-time system monitoring
./bin/monitoring_integration_sample

# Messaging patterns (pub/sub, request/reply)
./bin/messaging_integration_sample
```

> 💡 **Next Steps**: Explore [Detailed Examples](#-detailed-examples) below or check individual sample READMEs

---

## 📋 Sample Programs

### 1. **logging_sample**
**Purpose:** Demonstrates asynchronous logging with multiple output targets

**Key Features:**
- ✅ Console and file output
- ✅ Multiple log levels (DEBUG, INFO, WARNING, ERROR)
- ✅ Async processing (4.34M+ logs/sec)
- ✅ Thread-safe operations

**Quick Usage:**
```cpp
#include <logger/logger.h>

int main() {
    auto logger = create_logger();
    logger->info("Application started");
    logger->error("Error occurred", __FILE__, __LINE__);
    return 0;
}
```

**Learn More:** [logging_sample/README.md](logging_sample/)

---

### 2. **container_sample**
**Purpose:** Demonstrates type-safe data containers with serialization

**Key Features:**
- ✅ Type-safe variant storage (bool, int, string, bytes)
- ✅ Binary and JSON serialization
- ✅ Thread-safe operations
- ✅ SIMD-optimized (ARM NEON, x86 AVX)

**Quick Usage:**
```cpp
#include <container/container.h>

int main() {
    container::value_container data;
    data.set_value("user_id", 12345);
    data.set_value("username", "john_doe");

    auto json = data.serialize_json();
    return 0;
}
```

**Learn More:** [container_sample/README.md](container_sample/)

---

### 3. **threads_sample**
**Purpose:** Demonstrates priority-based thread pool with job scheduling

**Key Features:**
- ✅ Priority scheduling (RealTime, Batch, Background)
- ✅ Lock-free job queues (2.48M+ jobs/sec)
- ✅ Adaptive queue optimization
- ✅ Work-stealing architecture

**Quick Usage:**
```cpp
#include <thread/thread_pool.h>

int main() {
    auto pool = std::make_shared<thread::thread_pool>(8);
    pool->start();

    pool->submit_task([]() {
        // Your concurrent task
    });

    return 0;
}
```

**Learn More:** [threads_sample/README.md](threads_sample/)

---

### 4. **echo_server**
**Purpose:** Demonstrates TCP server implementation with async I/O

**Key Features:**
- ✅ Asynchronous connection handling
- ✅ Multi-client support
- ✅ Session management
- ✅ Configurable buffer sizes

**Quick Usage:**
```cpp
#include <network/messaging_server.h>

int main() {
    network::messaging_server server(8080);

    server.on_message([](auto& conn, const std::string& data) {
        conn->send("Echo: " + data);
    });

    server.start();
    return 0;
}
```

**Learn More:** [echo_server/README.md](echo_server/)

---

### 5. **echo_client**
**Purpose:** Demonstrates TCP client implementation with async I/O

**Key Features:**
- ✅ Asynchronous connection management
- ✅ Message queuing
- ✅ Automatic reconnection
- ✅ Timeout handling

**Quick Usage:**
```cpp
#include <network/messaging_client.h>

int main() {
    network::messaging_client client("localhost", 8080);
    client.start();

    client.send("Hello, Server!");

    client.on_message([](const std::string& data) {
        std::cout << "Received: " << data << std::endl;
    });

    return 0;
}
```

**Learn More:** [echo_client/README.md](echo_client/)

---

### 6. **udp_echo_server**
**Purpose:** Demonstrates UDP server implementation with datagram handling

**Key Features:**
- ✅ Connectionless UDP communication
- ✅ Endpoint-based routing
- ✅ Asynchronous datagram processing
- ✅ Multiple client support (stateless)

**Quick Usage:**
```cpp
#include "network_system/core/messaging_udp_server.h"

int main() {
    using namespace network_system::core;

    auto server = std::make_shared<messaging_udp_server>("UDPEchoServer");

    // Set up receive callback
    server->set_receive_callback(
        [server](const std::vector<uint8_t>& data,
                const asio::ip::udp::endpoint& sender)
        {
            std::string msg(data.begin(), data.end());
            std::cout << "Received: " << msg << "\n";

            // Echo back
            std::string echo = "Echo: " + msg;
            std::vector<uint8_t> response(echo.begin(), echo.end());

            server->async_send_to(std::move(response), sender,
                [](std::error_code ec, std::size_t bytes) {
                    if (!ec) std::cout << "Sent " << bytes << " bytes\n";
                });
        });

    server->start_server(5555);
    server->wait_for_stop();

    return 0;
}
```

**Learn More:** [udp_echo_server/README.md](udp_echo_server/)

---

### 7. **udp_echo_client**
**Purpose:** Demonstrates UDP client implementation with datagram transmission

**Key Features:**
- ✅ Connectionless UDP communication
- ✅ Automatic endpoint resolution
- ✅ Asynchronous send/receive
- ✅ Statistics tracking

**Quick Usage:**
```cpp
#include "network_system/core/messaging_udp_client.h"

int main() {
    using namespace network_system::core;

    auto client = std::make_shared<messaging_udp_client>("UDPEchoClient");

    // Set up receive callback
    client->set_receive_callback(
        [](const std::vector<uint8_t>& data,
          const asio::ip::udp::endpoint& sender)
        {
            std::string msg(data.begin(), data.end());
            std::cout << "Received: " << msg << "\n";
        });

    client->start_client("localhost", 5555);

    // Send datagram
    std::string msg = "Hello, UDP!";
    std::vector<uint8_t> data(msg.begin(), msg.end());

    client->send_packet(std::move(data),
        [](std::error_code ec, std::size_t bytes) {
            if (!ec) std::cout << "Sent " << bytes << " bytes\n";
        });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    client->stop_client();

    return 0;
}
```

**Learn More:** [udp_echo_client/README.md](udp_echo_client/)

---

### 8. **combined_sample**
**Purpose:** Demonstrates integration of multiple systems (Logger + Container + Threads)

**Key Features:**
- ✅ Multi-system integration patterns
- ✅ Async logging with thread pool
- ✅ Type-safe data containers for results
- ✅ Concurrent job processing

**Quick Usage:**
```cpp
#include "kcenon/logger/core/logger.h"
#include "kcenon/thread/core/thread_pool.h"
#include "container/container.h"

int main() {
    // Create logger
    auto log = std::make_shared<logger>(true, 8192);
    log->start();

    // Create thread pool
    auto pool = std::make_shared<thread_pool>("worker-pool");
    pool->start();

    // Submit jobs with logging
    for (int i = 0; i < 10; ++i) {
        auto job = std::make_unique<callback_job>(
            [i, log]() -> std::optional<std::string> {
                log->log(log_level::info,
                    fmt::format("Processing job {}", i));

                // Create result container
                value_container result;
                result.add(std::make_shared<value>("job_id",
                    value_types::int_value, std::to_string(i)));

                return std::nullopt; // Success
            }
        );
        pool->enqueue(std::move(job));
    }

    return 0;
}
```

**Learn More:** [combined_sample/README.md](combined_sample/)

---

### 9. **database_integration_sample**
**Purpose:** Demonstrates asynchronous database operations using database_system, thread_system, and logger_system

**Key Features:**
- ✅ SQLite database operations (in-memory and file-based)
- ✅ Async query execution with thread pool
- ✅ Query builder pattern for type-safe SQL
- ✅ Connection pooling and transaction management
- ✅ Real-time performance monitoring and statistics
- ✅ Comprehensive error handling and logging

**Quick Usage:**
```cpp
#include "database/database_manager.h"
#include "database/query_builder.h"
#include "kcenon/logger/core/logger.h"
#include "kcenon/thread/core/thread_pool.h"

int main() {
    // Initialize logger
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>());
    log->start();

    // Initialize database
    database::database_manager db_manager;
    db_manager.set_mode(database::database_types::sqlite);
    db_manager.connect(":memory:");

    // Create table
    auto builder = db_manager.create_query_builder();
    // ... execute CREATE TABLE query

    // Initialize thread pool
    auto pool = std::make_shared<thread_pool>("db-worker-pool");
    pool->start();

    // Submit async INSERT job
    pool->enqueue(std::make_unique<callback_job>(
        [&]() -> std::optional<std::string> {
            auto builder = db_manager.create_query_builder();
            // ... execute INSERT query
            log->log(log_level::info, "INSERT completed");
            return std::nullopt;
        },
        "insert-job-1"
    ));

    // Submit async SELECT job
    pool->enqueue(std::make_unique<callback_job>(
        [&]() -> std::optional<std::string> {
            auto builder = db_manager.create_query_builder();
            // ... execute SELECT query
            log->log(log_level::info, "SELECT completed");
            return std::nullopt;
        },
        "select-job-1"
    ));

    // Cleanup
    pool->stop();
    db_manager.disconnect();
    log->stop();

    return 0;
}
```

**Sample Output:**
```
═══════════════════════════════════════════════════════
  Phase 1: Component Initialization
═══════════════════════════════════════════════════════

[Logger] Logger system initialized
[Database] Connected to SQLite (:memory:)
[Database] Created 'users' table
[Thread Pool] Thread pool started

[System] All components initialized successfully

═══════════════════════════════════════════════════════
  Phase 2: Asynchronous Database Operations
═══════════════════════════════════════════════════════

[Phase 2] Submitting 10 INSERT and 10 SELECT operations
[Job #1] Executing INSERT query
[Job #1] INSERT completed (2ms)
[Job #2] Executing SELECT query
[Job #2] SELECT completed (1ms)
...

═══════════════════════════════════════════════════════
  Phase 4: Final Dashboard
═══════════════════════════════════════════════════════

╔═════════════════════════════════════════════════════════╗
║       Database Integration Dashboard                   ║
╚═════════════════════════════════════════════════════════╝

📊 Query Statistics:
   Total Queries:    20
   Successful:       20
   Failed:           0
   Success Rate:     100.0%

⏱️  Performance:
   Avg INSERT Time:  2 ms
   Avg SELECT Time:  1 ms

═══════════════════════════════════════════════════════════

✓ database_system:        SUCCESS (SQLite)
✓ thread_system:          SUCCESS (20 async operations)
✓ logger_system:          SUCCESS
✓ Success Rate:           100.0%
```

**Learn More:** [database_integration_sample/README.md](database_integration_sample/)

---

### 10. **thread_integration_sample**
**Purpose:** Demonstrates advanced thread_system features with priority-based scheduling and performance tracking

**Key Features:**
- ✅ Basic thread_pool for simple async operations
- ✅ Priority-based job scheduling (High/Normal/Low)
- ✅ Real-time performance monitoring and statistics
- ✅ Comprehensive logging with logger_system
- ✅ Different workload types (compute-intensive, I/O-bound)
- ✅ Min/Max/Average execution time tracking

**Quick Usage:**
```cpp
#include "kcenon/logger/core/logger.h"
#include "kcenon/thread/core/thread_pool.h"
#include "kcenon/thread/core/callback_job.h"

int main() {
    // Initialize logger
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>());
    log->start();

    // Create basic thread pool
    auto pool = std::make_shared<thread_pool>("worker-pool");
    pool->start();

    // Submit jobs with different priorities
    // High priority - compute-intensive
    for (int i = 0; i < 3; ++i) {
        auto job = std::make_unique<callback_job>(
            [log, i]() -> std::optional<std::string> {
                log->log(log_level::info,
                        fmt::format("[High Priority] Job {} executing", i));
                // Perform compute-intensive work
                return std::nullopt;
            },
            fmt::format("high-priority-{}", i)
        );
        pool->enqueue(std::move(job));
    }

    // Normal priority - balanced workload
    for (int i = 0; i < 3; ++i) {
        auto job = std::make_unique<callback_job>(
            [log, i]() -> std::optional<std::string> {
                log->log(log_level::info,
                        fmt::format("[Normal Priority] Job {} executing", i));
                return std::nullopt;
            },
            fmt::format("normal-priority-{}", i)
        );
        pool->enqueue(std::move(job));
    }

    // Low priority - I/O-bound
    for (int i = 0; i < 3; ++i) {
        auto job = std::make_unique<callback_job>(
            [log, i]() -> std::optional<std::string> {
                log->log(log_level::info,
                        fmt::format("[Low Priority] Job {} executing", i));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                return std::nullopt;
            },
            fmt::format("low-priority-{}", i)
        );
        pool->enqueue(std::move(job));
    }

    // Wait for completion
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Cleanup
    pool->stop();
    log->stop();

    return 0;
}
```

**Sample Output:**
```
═══════════════════════════════════════════════════════
  Phase 1: Component Initialization
═══════════════════════════════════════════════════════

[Logger] Logger system initialized

[System] All components initialized successfully

═══════════════════════════════════════════════════════
  Phase 2: Basic Thread Pool Operations
═══════════════════════════════════════════════════════

[Thread Pool] Basic pool started
[Phase 2] Submitting 5 basic jobs
[Basic Job #1] Executing
[Basic Job #1] Completed (15ms)
[Basic Job #2] Executing
[Basic Job #2] Completed (12ms)
...

═══════════════════════════════════════════════════════
  Phase 3: Priority-Based Thread Pool
═══════════════════════════════════════════════════════

[Thread Pool] Priority pool started
[Phase 3] Submitting 3 high, 3 normal, 3 low priority jobs
[High Priority Job #1] Executing
[High Priority Job #1] Completed (22ms)
[Normal Priority Job #1] Executing
[Normal Priority Job #1] Completed (18ms)
...

═══════════════════════════════════════════════════════
  Phase 5: Final Dashboard
═══════════════════════════════════════════════════════

╔═════════════════════════════════════════════════════════╗
║       Thread Integration Dashboard                    ║
╚═════════════════════════════════════════════════════════╝

📊 Job Statistics:
   Total Jobs:       14
   Completed:        14
   Failed:           0
   Success Rate:     100.0%

🎯 Priority Distribution:
   High Priority:    3
   Normal Priority:  3
   Low Priority:     3

⏱️  Performance:
   Avg Execution:    16.43 ms
   Min Execution:    12 ms
   Max Execution:    54 ms

═══════════════════════════════════════════════════════════

✓ thread_system:          SUCCESS (14 jobs processed)
✓ logger_system:          SUCCESS
✓ Priority Scheduling:    DEMONSTRATED
✓ Success Rate:           100.0%
```

**Learn More:** [thread_integration_sample/README.md](thread_integration_sample/)

---

### 11. **monitoring_integration_sample**
**Purpose:** Demonstrates real-time system monitoring and performance profiling with monitoring_system, thread_system, and logger_system integration

**Key Features:**
- ✅ System resource monitoring (CPU, memory, threads)
- ✅ Performance profiling with percentile metrics (P50/P95/P99)
- ✅ Real-time resource snapshots during execution
- ✅ Operation throughput tracking
- ✅ Integration with logger_system for event logging
- ✅ Comprehensive dashboard display

**Quick Usage:**
```cpp
#include "kcenon/logger/core/logger.h"
#include "kcenon/thread/core/thread_pool.h"
#include "kcenon/monitoring/core/performance_monitor.h"
#include "kcenon/monitoring/collectors/system_resource_collector.h"

int main() {
    // Initialize components
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>());
    log->start();

    performance_profiler profiler;
    system_info_collector resource_collector;

    auto pool = std::make_shared<thread_pool>("monitored-pool");
    pool->start();

    // Collect baseline system resources
    auto baseline = resource_collector.collect();
    log->log(log_level::info,
            fmt::format("CPU: {:.1f}%, Memory: {} MB",
                       baseline.cpu_usage_percent,
                       baseline.used_memory_bytes / (1024 * 1024)));

    // Execute monitored jobs
    for (int i = 0; i < 10; ++i) {
        auto job = std::make_unique<callback_job>(
            [&profiler, log, i]() -> std::optional<std::string> {
                auto start = std::chrono::high_resolution_clock::now();

                // Perform work
                volatile uint64_t result = 0;
                for (int j = 0; j < 100000; ++j) {
                    result += j * j;
                }

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    end - start);

                // Record performance sample
                profiler.record_sample("compute_job", duration);

                log->log(log_level::info,
                        fmt::format("Job {} completed", i));

                return std::nullopt;
            },
            fmt::format("job-{}", i)
        );
        pool->enqueue(std::move(job));
    }

    // Wait for completion
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Get performance metrics
    auto metrics = profiler.get_metrics();
    for (const auto& [name, metric] : metrics) {
        log->log(log_level::info,
                fmt::format("Operation: {}, Calls: {}, "
                           "Mean: {:.2f}ms, P95: {:.2f}ms",
                           name, metric.call_count,
                           metric.mean_duration.count() / 1e6,
                           metric.p95_duration.count() / 1e6));
    }

    // Collect final system resources
    auto final = resource_collector.collect();
    log->log(log_level::info,
            fmt::format("Final - CPU: {:.1f}%, Memory: {} MB",
                       final.cpu_usage_percent,
                       final.used_memory_bytes / (1024 * 1024)));

    // Cleanup
    pool->stop();
    log->stop();

    return 0;
}
```

**Sample Output:**
```
═══════════════════════════════════════════════════════
  Phase 2: Baseline System Resources
═══════════════════════════════════════════════════════

╔═════════════════════════════════════════════════════════╗
║       System Resources Dashboard                      ║
╚═════════════════════════════════════════════════════════╝

💻 CPU Metrics:
   CPU Usage:        12.5%
   CPU Count:        8
   Load Avg (1min):  2.34

💾 Memory Metrics:
   Total Memory:     16384 MB
   Used Memory:      8192 MB
   Available Memory: 8192 MB
   Memory Usage:     50.0%

🧵 Process Metrics:
   Thread Count:     24
   Process Count:    156

═══════════════════════════════════════════════════════════

═══════════════════════════════════════════════════════
  Phase 3: Execute Monitored Jobs
═══════════════════════════════════════════════════════

[Phase 3] Submitting 15 monitored jobs

[Job #1] Starting (complexity: 5)
[Job #1] Completed
[Job #2] Starting (complexity: 10)
[Job #2] Completed
...

═══════════════════════════════════════════════════════
  Phase 6: Performance Metrics
═══════════════════════════════════════════════════════

╔═════════════════════════════════════════════════════════╗
║       Performance Metrics Dashboard                   ║
╚═════════════════════════════════════════════════════════╝

📊 Operation: compute_job_0
   Call Count:       5
   Error Count:      0
   Min Duration:     12.34 ms
   Max Duration:     18.92 ms
   Mean Duration:    15.23 ms
   Median Duration:  14.87 ms
   P95 Duration:     18.45 ms
   P99 Duration:     18.89 ms
   Throughput:       65.50 ops/sec

═══════════════════════════════════════════════════════════

✓ monitoring_system:      SUCCESS (15 operations profiled)
✓ thread_system:          SUCCESS (15 jobs processed)
✓ logger_system:          SUCCESS
✓ Resource Snapshots:     5 collected
✓ Success Rate:           100.0%
```

**Learn More:** [monitoring_integration_sample/README.md](monitoring_integration_sample/)

---

### 12. **messaging_integration_sample**

**Complexity**: ⭐⭐⭐⭐ Expert | **LOC**: ~500

Comprehensive demonstration of **messaging_system + logger_system** integration showcasing asynchronous messaging patterns for distributed systems.

**Integration Architecture**:
```
┌─────────────────────────────────────────────────────┐
│         messaging_integration_sample                │
│                                                     │
│  ┌──────────────┐    ┌──────────────┐            │
│  │ Message Bus  │────│ Pub/Sub      │            │
│  │              │    │ Pattern      │            │
│  └──────┬───────┘    └──────────────┘            │
│         │                                          │
│         │            ┌──────────────┐            │
│         └────────────│ Request/     │            │
│                      │ Reply Pattern│            │
│                      └──────────────┘            │
│                                                     │
│  ┌──────────────┐                                  │
│  │Logger System │  (Observability)                │
│  └──────────────┘                                  │
└─────────────────────────────────────────────────────┘
```

**Key Features**:

1. **Pub/Sub Pattern**
   - Multiple topic subscriptions (orders, inventory, notifications, system broadcasts)
   - Priority-based message routing
   - Asynchronous message distribution
   - Topic-based filtering

2. **Request/Reply Pattern**
   - Synchronous request/response with timeout
   - Future-based async response handling
   - Service query/response simulation
   - Error handling and retry logic

3. **Message Features**
   - Message priorities (low, normal, high, critical)
   - Message types (request, response, notification, broadcast)
   - Metadata (sender, recipient, timestamp, headers)
   - Variant-based payload (string, int64, double, bool, binary)

4. **Statistics & Monitoring**
   - Messages published/processed/failed counters
   - Active subscriptions tracking
   - Pending requests monitoring
   - Custom metrics for different patterns

**Usage**:
```bash
cd build
./bin/messaging_integration_sample
```

**Expected Output**:
```
═══════════════════════════════════════════════════════
  Messaging Integration Sample
  (messaging + logger systems)
═══════════════════════════════════════════════════════

[Phase 1] Component Initialization...
[Logger] Logger system initialized
[Message Bus] Message bus initialized
[Message Bus] Worker threads: 4
[Message Bus] Max queue size: 10000

[Phase 2] Pub/Sub Pattern - Subscribe to Topics...
[Phase 2] Subscribed to 4 topics

[Phase 3] Pub/Sub Pattern - Publish Messages...
[Publisher] Published order message 1
[Subscriber:Orders] Received order from: OrderService
[Subscriber:Orders] Order ID: ORD-0001, Amount: $99.99
...
[Phase 3] Published 12 messages

[Phase 4] Request/Reply Pattern...
[Client] Sending request #1
[Service] Received request: <id>
[Service] Query: SELECT * FROM table_1
[Client] Response #1: Processed: SELECT * FROM table_1 (success)
...
[Phase 4] Sent 3 requests, received 3 responses

[Phase 5] Final Statistics...
╔═════════════════════════════════════════════════════════╗
║       Message Bus Statistics Dashboard               ║
╚═════════════════════════════════════════════════════════╝

📊 Message Bus Metrics:
   Published:         15
   Processed:         15
   Failed:            0
   Active Subs:       4
   Pending Requests:  0

📈 Custom Metrics:
   Total Published:   15
   Total Received:    12
   Requests Sent:     3
   Responses Recv:    3
   Notifications:     3
   Broadcasts:        1

Integration Summary:
✓ messaging_system:       SUCCESS (15 messages published)
✓ logger_system:          SUCCESS
✓ Pub/Sub Pattern:        12 messages received
✓ Request/Reply Pattern:  3/3 responses
✓ Topics:                 4 active
```

**Code Example: Pub/Sub Pattern**
```cpp
// Subscribe to a topic
bus->subscribe("orders", [](const message& msg) {
    auto order_id = msg.payload.get<std::string>("order_id");
    auto amount = msg.payload.get<double>("amount");

    std::cout << "Order " << order_id << ": $" << amount << std::endl;
});

// Publish message to topic
auto msg = message("orders", "OrderService");
msg.payload.set("order_id", "ORD-0001");
msg.payload.set("amount", 99.99);
msg.metadata.priority = message_priority::high;

bus->publish(msg);
```

**Code Example: Request/Reply Pattern**
```cpp
// Service responds to requests
bus->subscribe("service.query", [&bus](const message& request_msg) {
    auto query = request_msg.payload.get<std::string>("query");

    // Process query...

    message response("service.response", "QueryService");
    response.metadata.type = message_type::response;
    response.payload.set("result", "Processed: " + query);
    response.payload.set("status", "success");

    bus->respond(request_msg, response);
});

// Client sends request
auto request_msg = message("service.query", "ClientService");
request_msg.metadata.type = message_type::request;
request_msg.metadata.timeout = std::chrono::milliseconds(5000);
request_msg.payload.set("query", "SELECT * FROM users");

auto future_response = bus->request(request_msg);

// Wait for response
if (future_response.wait_for(std::chrono::seconds(3)) == std::future_status::ready) {
    auto response = future_response.get();
    auto result = response.payload.get<std::string>("result");
    std::cout << "Result: " << result << std::endl;
}
```

**Educational Value**:
- ✅ Asynchronous messaging patterns (Pub/Sub, Request/Reply)
- ✅ Message routing and topic-based distribution
- ✅ Priority-based message processing
- ✅ Timeout handling and error recovery
- ✅ Message serialization and payload management
- ✅ Distributed system communication patterns
- ✅ Real-time statistics and monitoring

**Real-World Applications**:
- Microservices communication (event-driven architecture)
- Message queue systems (RabbitMQ, Kafka patterns)
- Distributed task processing
- Event sourcing and CQRS patterns
- Real-time notification systems
- Service orchestration and choreography

**Learn More:** [messaging_integration_sample/README.md](messaging_integration_sample/)

---

## 🔥 Detailed Examples

### Example 1: Complete Logging Pipeline

Shows integration of **logger_system** with various output targets:

```cpp
#include <logger/logger.h>
#include <logger/writers/console_writer.h>
#include <logger/writers/file_writer.h>

int main() {
    // Create logger with builder pattern
    auto logger = logger::logger_builder()
        .use_template("production")
        .add_writer("console", std::make_unique<logger::console_writer>())
        .add_writer("file", std::make_unique<logger::file_writer>("app.log"))
        .build()
        .value();

    // Log different levels
    logger->log(logger::log_level::info, "Application started");
    logger->log(logger::log_level::debug, "Debug information");
    logger->log(logger::log_level::error, "Error occurred");

    return 0;
}
```

### Example 2: Thread Pool with Monitoring

Shows integration of **thread_system** with **monitoring_system**:

```cpp
#include <thread/thread_pool.h>
#include <monitoring/metrics_collector.h>

int main() {
    // Create monitored thread pool
    auto pool = std::make_shared<thread::thread_pool>(8);
    auto metrics = std::make_shared<monitoring::metric_collector>();

    pool->start();

    // Submit batch of jobs
    for (int i = 0; i < 10000; ++i) {
        pool->submit_task([i, &metrics]() {
            // Process work
            process_item(i);

            // Update metrics
            metrics->record_counter("jobs_completed", 1);
        });
    }

    // Get performance metrics
    auto snapshot = metrics->export_prometheus();
    std::cout << snapshot << std::endl;

    return 0;
}
```

### Example 3: Network Communication with Containers

Shows integration of **network_system** with **container_system**:

```cpp
#include <network/messaging_server.h>
#include <container/value_container.h>

int main() {
    network::messaging_server server(8080);

    server.on_message([](auto& conn, const std::string& data) {
        // Deserialize container from network
        container::value_container msg;
        msg.deserialize(data);

        // Process message
        auto user_id = msg.get_value("user_id").value<int>();
        auto action = msg.get_value("action").value<std::string>();

        // Send response
        container::value_container response;
        response.set_value("status", "success");
        response.set_value("user_id", user_id);

        conn->send(response.serialize());
    });

    server.start();
    return 0;
}
```

### Example 4: Multi-System Integration (Combined Sample)

Shows integration of **logger_system**, **thread_system**, and **container_system**:

```cpp
#include "fmt/format.h"
#include "kcenon/logger/core/logger.h"
#include "kcenon/logger/writers/console_writer.h"
#include "kcenon/thread/core/thread_pool.h"
#include "kcenon/thread/core/callback_job.h"
#include "container/container.h"

using kcenon::logger::logger;
using kcenon::logger::console_writer;
using kcenon::thread::thread_pool;
using kcenon::thread::callback_job;
using container_module::value_container;
using container_module::value;
using container_module::value_types;

int main() {
    // Create logger with async support
    auto log = std::make_shared<logger>(true, 8192);
    log->add_writer(std::make_unique<console_writer>());
    log->start();

    // Create thread pool for concurrent processing
    auto pool = std::make_shared<thread_pool>("worker-pool");
    pool->start();

    // Submit jobs that integrate all three systems
    std::atomic<int> jobs_completed{0};

    for (int i = 0; i < 10; ++i) {
        auto job = std::make_unique<callback_job>(
            [i, log, &jobs_completed]() -> std::optional<std::string> {
                // Log the processing (logger_system)
                log->log(kcenon::thread::log_level::info,
                    fmt::format("[Worker] Processing job {}", i));

                // Simulate work (thread_system)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Store result in container (container_system)
                value_container result;
                result.add(std::make_shared<value>("job_id",
                    value_types::int_value, std::to_string(i)));
                result.add(std::make_shared<value>("status",
                    value_types::string_value, "completed"));
                result.add(std::make_shared<value>("result",
                    value_types::int_value, std::to_string(i * 10)));

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

    // Wait for completion
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << fmt::format("Jobs Completed: {}\n", jobs_completed.load());

    // Cleanup
    pool->stop();
    log->stop();

    return 0;
}
```

**What this demonstrates:**
- **Logger System**: Async logging from multiple threads without blocking
- **Thread System**: Concurrent job execution with callback-based tasks
- **Container System**: Type-safe data storage for results
- **Integration**: How three systems work together seamlessly

**Output:**
```
[2025-10-24 10:17:19.299] [INFO] [Worker] Processing job 0
[2025-10-24 10:17:19.300] [INFO] [Worker] Processing job 1
...
[2025-10-24 10:17:19.450] [INFO] [Worker] Job 0 completed with result: 0
Jobs Completed: 10
```

---

## 🛠️ Building from Source

### Prerequisites

- **Compiler**: C++20 compatible (GCC 11+, Clang 14+, MSVC 2019+)
- **CMake**: 3.16 or later
- **Git**: For cloning the repository
- **vcpkg** (optional): Automatically detected if available

### Build All Samples

```bash
# Clean build (default: FetchContent mode)
./build.sh --clean

# Debug build
./build.sh --debug

# Release build
./build.sh --release
```

### Build with Different Dependency Modes

```bash
# Mode 1: FetchContent (default) - Downloads dependencies automatically
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Mode 2: Local systems - Uses local repositories from ../
cmake -B build -DSAMPLES_USE_LOCAL_SYSTEMS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Mode 3: Installed packages - Uses system-installed libraries
cmake -B build -DSAMPLES_USE_INSTALLED_SYSTEMS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Build Individual Sample

```bash
# First configure the project
cmake -B build

# Then build specific target
cmake --build build --target logging_sample -j
./build/bin/logging_sample
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `SAMPLES_USE_LOCAL_SYSTEMS` | OFF | Use local system repositories |
| `SAMPLES_USE_INSTALLED_SYSTEMS` | OFF | Use installed system packages |
| `SAMPLES_BUILD_EXAMPLES` | ON | Build all sample programs |
| `CMAKE_BUILD_TYPE` | Release | Build type (Debug/Release) |

### Dependency Resolution

The build system automatically resolves dependencies in the following order:

1. **FetchContent Mode** (default):
   - Downloads from GitHub: common_system, thread_system, logger_system, container_system, network_system, messaging_system
   - No manual setup required

2. **Local Mode** (`-DSAMPLES_USE_LOCAL_SYSTEMS=ON`):
   - Looks for systems in `../common_system`, `../thread_system`, etc.
   - Useful for development across multiple systems

3. **Installed Mode** (`-DSAMPLES_USE_INSTALLED_SYSTEMS=ON`):
   - Uses `find_package()` to locate installed libraries
   - Requires systems to be installed via CMake install

---

## 🌐 Platform Support

| Platform | Compiler | Status | Notes |
|----------|----------|--------|-------|
| **Ubuntu 22.04+** | GCC 11+ | ✅ Fully Tested | Primary development platform |
| **Ubuntu 22.04+** | Clang 14+ | ✅ Fully Tested | Full C++20 support |
| **Windows 10+** | MSVC 2019+ | ✅ Tested | Visual Studio 2019/2022 |
| **Windows 10+** | MSYS2 (GCC) | ✅ Tested | MinGW-w64 toolchain |
| **macOS 13+** | Apple Clang | ✅ Tested | Intel and Apple Silicon |

---

## 📊 Performance Characteristics

### Sample Execution Times (Typical)

| Sample | Execution Time | Operations | Throughput |
|--------|----------------|------------|------------|
| **logging_sample** | ~50ms | 10,000 logs | 200K logs/sec |
| **container_sample** | ~30ms | 10,000 operations | 333K ops/sec |
| **threads_sample** | ~100ms | 100,000 jobs | 1M jobs/sec |
| **echo_server** (TCP) | N/A (service) | - | 10K+ connections |
| **echo_client** (TCP) | ~10ms | 1,000 messages | 100K msg/sec |
| **udp_echo_server** | N/A (service) | - | 100K+ datagrams/sec |
| **udp_echo_client** | ~3s | 5 datagrams | Lower latency than TCP |
| **combined_sample** | ~2s | 10 async jobs | 5 jobs/sec |

*Benchmarked on Apple M1 (8-core) @ 3.2GHz*

**Note:** UDP typically has lower per-packet overhead and latency compared to TCP, but doesn't guarantee delivery or order.

---

## 📚 Documentation

- **[API Reference](docs/API_REFERENCE.md)** - Complete API documentation
- **[Build Guide](docs/BUILD_GUIDE.md)** - Detailed build instructions
- **[Architecture](docs/ARCHITECTURE.md)** - System design overview

### Component Documentation

For detailed documentation of each component, see:
- [common_system](https://github.com/kcenon/common_system) - Foundation interfaces
- [thread_system](https://github.com/kcenon/thread_system) - Threading framework
- [logger_system](https://github.com/kcenon/logger_system) - Async logging
- [monitoring_system](https://github.com/kcenon/monitoring_system) - Metrics collection
- [container_system](https://github.com/kcenon/container_system) - Data containers
- [network_system](https://github.com/kcenon/network_system) - Network communication
- [messaging_system](https://github.com/kcenon/messaging_system) - Complete integration

---

## 🔗 Related Projects

### For Enterprise Solutions

If you need a complete, pre-integrated threading solution with built-in observability and zero configuration:

- **[integrated_thread_system](https://github.com/kcenon/integrated_thread_system)** -
  Enterprise-grade unified threading framework combining thread_system, logger_system,
  and monitoring_system. Provides a high-level `unified_thread_system` API with:
  - ✅ Zero-configuration setup
  - ✅ Built-in async logging (4.34M+ logs/sec)
  - ✅ Real-time monitoring and health checks
  - ✅ Lock-free thread pools (2.48M+ jobs/sec)

  **Use this if:** You want a complete solution without managing individual systems.

  **Use samples if:** You want to learn how each system works independently and build custom integrations.

---

## 🤝 Contributing

We welcome contributions! To add a new sample:

1. Fork the repository
2. Create a new directory under `samples/`
3. Follow the existing sample structure
4. Add README.md with usage instructions
5. Update this main README with your sample
6. Submit a Pull Request

### Sample Template Structure

```
your_sample/
├── CMakeLists.txt
├── README.md
├── src/
│   └── main.cpp
└── include/
    └── your_sample.h
```

---

## 📄 License

**BSD 3-Clause License**

Copyright (c) 2021-2025, kcenon (🍀☀🌕🌥 🌊)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

---

## 📬 Contact & Support

- **Author**: kcenon ([@kcenon](https://github.com/kcenon))
- **Email**: kcenon@gmail.com
- **Issues**: [GitHub Issues](https://github.com/kcenon/samples/issues)
- **Discussions**: [GitHub Discussions](https://github.com/kcenon/samples/discussions)

---

<div align="center">

**Learn by example. Build with confidence.**

Made with ❤️ by the Open Source Community

</div>
