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
| **[echo_server](#4-echo_server)** | network_system | ~180 | ⭐⭐ Medium | TCP server implementation |
| **[echo_client](#5-echo_client)** | network_system | ~150 | ⭐⭐ Medium | TCP client implementation |
| **[combined_sample](#6-combined_sample)** | Multi-system | ~145 | ⭐⭐⭐ Advanced | Logger + Container + Threads integration |

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

# Network samples - Client/Server communication
./bin/echo_server &    # Start server in background
./bin/echo_client      # Run client

# Combined integration sample - Multi-system usage
./bin/combined_sample
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

### 6. **combined_sample**
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
| **echo_server** | N/A (service) | - | 10K+ connections |
| **echo_client** | ~10ms | 1,000 messages | 100K msg/sec |
| **combined_sample** | ~2s | 10 async jobs | 5 jobs/sec |

*Benchmarked on Apple M1 (8-core) @ 3.2GHz*

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
