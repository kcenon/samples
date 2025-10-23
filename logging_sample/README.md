# Logging Sample

## Overview

This sample demonstrates the logger_system features including:

- Multi-threaded logging with async processing
- Console-based logging output
- Thread-safe log writing from multiple threads
- Simple logger configuration

## Building

```bash
cd /path/to/samples
mkdir -p build && cd build
cmake .. -DSAMPLES_USE_LOCAL_SYSTEMS=ON
make logging_sample
```

## Running

```bash
cd build
./bin/logging_sample
```

## Expected Output

```
Logging sample starting...
Using default configuration (log_level=Information, style=ConsoleOnly)
Logger started: logging_sample
[INFO] Test_from_thread_0: 0
[INFO] Test_from_thread_1: 0
[INFO] Test_from_thread_2: 0
[INFO] Test_from_thread_0: 1
[INFO] Test_from_thread_1: 1
[INFO] Test_from_thread_2: 1
[INFO] Test_from_thread_0: 2
[INFO] Test_from_thread_1: 2
[INFO] Test_from_thread_2: 2
...
Logger stopped: logging_sample
```

## Features Demonstrated

### 1. Logger Initialization

```cpp
auto logger = std::make_shared<kcenon::logger::logger>(true, 8192);
logger->add_writer(std::make_unique<kcenon::logger::console_writer>());
logger->start();
```

- Creates async logger with 8KB buffer
- Adds console writer for output
- Starts the logging system

### 2. Multi-threaded Logging

```cpp
std::vector<std::thread> threads;
for (int i = 0; i < 3; ++i) {
    threads.emplace_back([logger, i]() {
        for (int j = 0; j < 5; ++j) {
            logger->log(kcenon::logger::log_level::information,
                fmt::format("Test_from_thread_{}: {}", i, j));
        }
    });
}
```

- Spawns multiple threads
- Each thread logs messages concurrently
- All logging operations are thread-safe

### 3. Log Levels

The logger supports multiple log levels:

- `trace` - Detailed debugging information
- `debug` - Debug-level messages
- `information` - General informational messages
- `warning` - Warning messages
- `error` - Error messages
- `fatal` - Fatal error messages

## Key Concepts

- **Async Logging**: Log messages are queued and written asynchronously to avoid blocking threads
- **Thread Safety**: Multiple threads can log safely without data races or corruption
- **Buffer Management**: Internal buffer (8KB) stores messages before flushing to output
- **Writer Pattern**: Multiple writers (console, file, etc.) can be attached to a single logger

## Architecture

```
Application Threads
    ↓ (log calls)
Logger (async queue)
    ↓ (batched writes)
Writers (console_writer, file_writer, etc.)
    ↓
Output (console, files, network, etc.)
```

## Related Samples

- [Container Sample](../container_sample/README.md) - Data serialization and deserialization
- [Threads Sample](../threads_sample/README.md) - Thread pool and priority-based job scheduling
- [Echo Server](../echo_server/README.md) - Network server using logger for diagnostics
