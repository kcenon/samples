/*****************************************************************************
BSD 3-Clause License

Copyright (c) 2021, 🍀☀🌕🌥 🌊
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
*****************************************************************************/

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "fmt/format.h"
#include "fmt/xchar.h"

#include "argument_parser.h"

constexpr auto PROGRAM_NAME = L"thread_sample";

using namespace utility_module;

// Define simple thread priority levels
enum class Priority { Low, Normal, High };

// Simple logger for the sample
enum class LogLevel { Debug, Information, Warning, Error, Parameter };
enum class LogStyle { ConsoleOnly, FileOnly, FileAndConsole };

#ifdef _DEBUG
LogLevel log_level = LogLevel::Parameter;
LogStyle log_style = LogStyle::ConsoleOnly;
#else
LogLevel log_level = LogLevel::Information;
LogStyle log_style = LogStyle::FileOnly;
#endif

// Simple logger implementation
class SimpleLogger {
private:
    LogLevel level_;
    LogStyle style_;
    std::wstring name_;
    bool active_ = false;
    std::mutex log_mutex_;

public:
    SimpleLogger() : level_(LogLevel::Information), style_(LogStyle::ConsoleOnly) {}
    
    void set_level(LogLevel level) { level_ = level; }
    void set_style(LogStyle style) { style_ = style; }
    
    void start(const std::wstring& name) {
        name_ = name;
        active_ = true;
        write(LogLevel::Information, L"Logger started");
    }
    
    void stop() {
        if (active_) {
            write(LogLevel::Information, L"Logger stopped");
            active_ = false;
        }
    }
    
    void write(LogLevel msg_level, const std::wstring& message) {
        if (!active_ || msg_level < level_) return;
        
        if (style_ != LogStyle::FileOnly) {
            std::lock_guard<std::mutex> lock(log_mutex_);
            std::wcout << get_level_prefix(msg_level) << message << std::endl;
        }
    }
    
private:
    std::wstring get_level_prefix(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return L"[DEBUG] ";
            case LogLevel::Information: return L"[INFO] ";
            case LogLevel::Warning: return L"[WARN] ";
            case LogLevel::Error: return L"[ERROR] ";
            case LogLevel::Parameter: return L"[PARAM] ";
            default: return L"[UNKNOWN] ";
        }
    }
};

// Simple job class
class Job {
public:
    using JobFunction = std::function<void()>;
    
    Job(Priority priority, JobFunction func) 
        : priority_(priority), function_(func) {}
    
    void execute() {
        if (function_) {
            function_();
        }
    }
    
    Priority get_priority() const { return priority_; }
    
private:
    Priority priority_;
    JobFunction function_;
};

// Simple thread pool implementation
class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::shared_ptr<Job>> jobs_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_ = false;
    SimpleLogger& logger_;

public:
    ThreadPool(SimpleLogger& logger, size_t threads = 4) 
        : logger_(logger) {
        for (size_t i = 0; i < threads; ++i) {
            workers_.emplace_back([this, i] {
                worker_loop(i);
            });
        }
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    void push_job(std::shared_ptr<Job> job) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            jobs_.push(job);
        }
        condition_.notify_one();
    }
    
private:
    void worker_loop(size_t worker_id) {
        while (true) {
            std::shared_ptr<Job> job;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this] { 
                    return stop_ || !jobs_.empty(); 
                });
                
                if (stop_ && jobs_.empty()) {
                    return;
                }
                
                job = jobs_.front();
                jobs_.pop();
            }
            
            if (job) {
                logger_.write(LogLevel::Information, 
                    fmt::format(L"Worker {} executing job with priority {}", 
                        worker_id, static_cast<int>(job->get_priority())));
                
                job->execute();
            }
        }
    }
};

bool parse_arguments(argument_manager& arguments);
void display_help(void);

int main(int argc, char* argv[])
{
    std::wcout << L"Threads sample starting..." << std::endl;
    
    // Set defaults
    log_level = LogLevel::Information;
    log_style = LogStyle::ConsoleOnly;
    
    if (argc > 1) {
        argument_manager arguments;
        auto result = arguments.try_parse(argc, argv);
        if (result.has_value()) {
            std::wcout << L"Argument parsing failed: " << std::wstring(result.value().begin(), result.value().end()) << std::endl;
            return 0;
        }
        
        if (!parse_arguments(arguments)) {
            return 0;
        }
    } else {
        std::wcout << L"No arguments provided, using defaults" << std::endl;
    }
    
    std::wcout << L"Creating logger..." << std::endl;
    SimpleLogger logger;
    logger.set_level(log_level);
    logger.set_style(log_style);
    logger.start(PROGRAM_NAME);

    // Create thread pool with 6 threads
    ThreadPool pool(logger, 6);
    
    // Example text data we'll use in jobs
    std::vector<std::wstring> test_messages = {
        L"Task 1 - High priority", 
        L"Task 2 - Normal priority", 
        L"Task 3 - Low priority"
    };
    
    // Add some jobs of various priorities
    for (int i = 0; i < 10; ++i) {
        // High priority jobs
        pool.push_job(std::make_shared<Job>(
            Priority::High, 
            [&logger, i, &test_messages]() {
                logger.write(LogLevel::Information, 
                    fmt::format(L"High Priority Job {}: {}", i, test_messages[0]));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        ));
        
        // Normal priority jobs
        pool.push_job(std::make_shared<Job>(
            Priority::Normal, 
            [&logger, i, &test_messages]() {
                logger.write(LogLevel::Information, 
                    fmt::format(L"Normal Priority Job {}: {}", i, test_messages[1]));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        ));
        
        // Low priority jobs
        pool.push_job(std::make_shared<Job>(
            Priority::Low, 
            [&logger, i, &test_messages]() {
                logger.write(LogLevel::Information, 
                    fmt::format(L"Low Priority Job {}: {}", i, test_messages[2]));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        ));
    }
    
    // Wait a bit to let jobs run
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    logger.stop();
    return 0;
}

bool parse_arguments(argument_manager& arguments)
{
    std::wstring temp;

    auto string_target = arguments.to_string("--help");
    if (string_target.has_value())
    {
        display_help();
        return false;
    }

    auto int_target = arguments.to_int("--logging_level");
    if (int_target.has_value())
    {
        int level = int_target.value();
        if (level >= 0 && level <= 4) {
            log_level = static_cast<LogLevel>(level);
        }
    }

    auto bool_target = arguments.to_bool("--write_console_only");
    if (bool_target.has_value() && bool_target.value())
    {
        log_style = LogStyle::ConsoleOnly;
        return true;
    }

    bool_target = arguments.to_bool("--write_console");
    if (bool_target.has_value() && bool_target.value())
    {
        log_style = LogStyle::FileAndConsole;
        return true;
    }

    log_style = LogStyle::FileOnly;
    return true;
}

void display_help(void)
{
    std::wcout << L"Thread sample options:" << std::endl << std::endl;
    std::wcout << L"--write_console [value] " << std::endl;
    std::wcout << L"\tThe write_console_mode on/off. If you want to display log on "
            L"console must be appended '--write_console true'.\n\tInitialize "
            L"value is --write_console off."
        << std::endl
        << std::endl;
    std::wcout << L"--logging_level [value]" << std::endl;
    std::wcout << L"\tIf you want to change log level must be appended "
            L"'--logging_level [level]'."
        << std::endl;
}