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
#include <string>
#include <thread>
#include <vector>
#include <chrono>

#include "fmt/format.h"
#include "fmt/xchar.h"

#include "argument_parser.h"

constexpr auto PROGRAM_NAME = L"logging_sample";

using namespace utility_module;

// Define logging levels and styles similar to what would be in a logger
enum class LogLevel { Debug, Information, Warning, Error, Fatal };
enum class LogStyle { ConsoleOnly, FileOnly, FileAndConsole };

#ifdef _DEBUG
LogLevel log_level = LogLevel::Debug;
LogStyle logging_style = LogStyle::ConsoleOnly;
#else
LogLevel log_level = LogLevel::Information;
LogStyle logging_style = LogStyle::FileOnly;
#endif

bool parse_arguments(argument_manager& arguments);
void display_help(void);

// Simple console logger implementation for sample purposes
class SimpleLogger {
private:
    LogLevel level_;
    LogStyle style_;
    std::wstring name_;
    bool active_ = false;

public:
    SimpleLogger() : level_(LogLevel::Information), style_(LogStyle::ConsoleOnly) {}
    
    void set_level(LogLevel level) { level_ = level; }
    void set_style(LogStyle style) { style_ = style; }
    
    void start(const std::wstring& name) {
        name_ = name;
        active_ = true;
        std::wcout << L"Logger started: " << name_ << std::endl;
    }
    
    void stop() {
        if (active_) {
            std::wcout << L"Logger stopped: " << name_ << std::endl;
            active_ = false;
        }
    }
    
    template<typename... Args>
    void write(LogLevel msg_level, const std::wstring& message) {
        if (!active_ || msg_level < level_) return;
        
        if (style_ != LogStyle::FileOnly) {
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
            case LogLevel::Fatal: return L"[FATAL] ";
            default: return L"[UNKNOWN] ";
        }
    }
};

int main(int argc, char* argv[])
{
    std::wcout << L"Logging sample starting..." << std::endl;
    
    // Set defaults
    log_level = LogLevel::Information;
    logging_style = LogStyle::ConsoleOnly;
    
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
    
    SimpleLogger logger;
    logger.set_level(log_level);
    logger.set_style(logging_style);
    logger.start(PROGRAM_NAME);

    std::vector<std::thread> threads;
    for (unsigned short thread_index = 0; thread_index < 3; ++thread_index)
    {
        threads.push_back(std::thread(
            [&logger, thread_index]()
            {
                for (unsigned int log_index = 0; log_index < 5; ++log_index)
                {
                    logger.write(
                        LogLevel::Information,
                        fmt::format(L"Test_from_thread_{}: {}", thread_index, log_index));
                    
                    // Add a small delay to make output readable
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

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
        logging_style = LogStyle::ConsoleOnly;
        return true;
    }

    bool_target = arguments.to_bool("--write_console");
    if (bool_target.has_value() && bool_target.value())
    {
        logging_style = LogStyle::FileAndConsole;
        return true;
    }

    logging_style = LogStyle::FileOnly;
    return true;
}

void display_help(void)
{
    std::wcout << L"Logging sample options:" << std::endl << std::endl;
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