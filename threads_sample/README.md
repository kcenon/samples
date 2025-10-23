# Threads Sample

## Overview

This sample demonstrates the thread_system features including:

- Priority-based thread pool with worker threads
- Job scheduling with three priority levels (High, Normal, Low)
- Multi-threaded task execution
- Thread-safe job queue management
- Worker affinity (workers can handle specific priority levels)

## Building

```bash
cd /path/to/samples
mkdir -p build && cd build
cmake .. -DSAMPLES_USE_LOCAL_SYSTEMS=ON
make threads_sample
```

## Running

```bash
cd build
./bin/threads_sample
```

## Expected Output

```
Threads sample starting...
Using default configuration (log_level=Information, style=ConsoleOnly)
Creating logger...
[INFO] Logger started
[INFO] Worker 3 executing job with priority 2
[INFO] High Priority Job 0: Task 1 - High priority
[INFO] Worker 5 executing job with priority 0
[INFO] Low Priority Job 1: Task 3 - Low priority
[INFO] Worker 1 executing job with priority 0
[INFO] Low Priority Job 0: Task 3 - Low priority
[INFO] Worker 4 executing job with priority 1
[INFO] Normal Priority Job 1: Task 2 - Normal priority
[INFO] Worker 0 executing job with priority 2
[INFO] High Priority Job 1: Task 1 - High priority
[INFO] Worker 2 executing job with priority 1
[INFO] Normal Priority Job 0: Task 2 - Normal priority
...
[INFO] Logger stopped
```

## Features Demonstrated

### 1. Thread Pool Creation

```cpp
// Create thread pool with 6 workers
auto pool = std::make_shared<kcenon::thread::thread_pool>("worker-pool", 6);
pool->start();
```

### 2. Priority-Based Job Submission

```cpp
// High priority jobs (executed first)
for (int i = 0; i < 10; ++i) {
    auto job = std::make_shared<callback_job>(
        priority_levels::high,
        [i]() {
            std::cout << "High Priority Job " << i << std::endl;
            return true;
        }
    );
    pool->enqueue(job);
}

// Normal priority jobs
for (int i = 0; i < 10; ++i) {
    auto job = std::make_shared<callback_job>(
        priority_levels::normal,
        [i]() {
            std::cout << "Normal Priority Job " << i << std::endl;
            return true;
        }
    );
    pool->enqueue(job);
}

// Low priority jobs (executed last)
for (int i = 0; i < 10; ++i) {
    auto job = std::make_shared<callback_job>(
        priority_levels::low,
        [i]() {
            std::cout << "Low Priority Job " << i << std::endl;
            return true;
        }
    );
    pool->enqueue(job);
}
```

### 3. Job Types

The sample demonstrates different job types:

- **Callback Jobs**: Jobs that wrap a lambda or function
- **Custom Jobs**: Jobs that inherit from the `job` base class

## Priority Levels

- **High (2)**: Urgent tasks, executed first
- **Normal (1)**: Standard tasks, executed after high priority
- **Low (0)**: Background tasks, executed when higher priorities are done

## Key Concepts

- **Priority Queue**: Jobs are stored in a priority queue and executed based on priority
- **Thread Pool**: Fixed number of worker threads process jobs from the queue
- **Thread Safety**: All queue operations are thread-safe with lock-free implementation
- **Fair Scheduling**: Within the same priority level, jobs are processed in FIFO order
- **Worker Affinity**: Workers can be configured to handle specific priority levels

## Architecture

```
Application
    ↓ (enqueue jobs)
Priority Job Queue
    ├── High Priority Queue [Job, Job, Job]
    ├── Normal Priority Queue [Job, Job, Job]
    └── Low Priority Queue [Job, Job, Job]
    ↓ (dequeue by priority)
Thread Pool Workers
    ├── Worker 0 (can handle: High, Normal, Low)
    ├── Worker 1 (can handle: High, Normal, Low)
    ├── Worker 2 (can handle: High, Normal, Low)
    ├── Worker 3 (can handle: High, Normal, Low)
    ├── Worker 4 (can handle: High, Normal, Low)
    └── Worker 5 (can handle: High, Normal, Low)
```

## Use Cases

- **Task Scheduling**: Execute tasks in order of importance
- **Resource Management**: Prioritize critical operations over background tasks
- **Load Balancing**: Distribute work across multiple threads
- **Async Operations**: Offload work from main thread to background threads

## Performance Characteristics

- **Lock-Free Queue**: High throughput with minimal contention
- **Scalable**: Efficient with many workers and many jobs
- **Low Latency**: High priority jobs start quickly
- **Fair**: Jobs at the same priority level are processed fairly

## Related Samples

- [Logging Sample](../logging_sample/README.md) - Uses thread pool for async logging
- [Container Sample](../container_sample/README.md) - Data structures used in job payloads
- [Echo Server](../echo_server/README.md) - Uses thread pool for handling connections
