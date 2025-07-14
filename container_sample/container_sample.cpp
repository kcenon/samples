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

#include "utilities/parsing/argument_parser.h"

#include "fmt/format.h"
#include "fmt/xchar.h"

#include <iostream>
#include <limits.h>
#include <memory>
#include <string>
#include <chrono>

constexpr auto PROGRAM_NAME = L"container_sample";

using namespace utility_module;

// Define a simple utility class for time measurement
class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_ms() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(now - start_time_).count();
    }
};

enum class LogLevel { Debug, Information, Warning, Error, Parameter };
enum class LogStyle { ConsoleOnly, FileOnly, FileAndConsole };

#ifdef _DEBUG
LogLevel log_level = LogLevel::Parameter;
LogStyle log_style = LogStyle::ConsoleOnly;
#else
LogLevel log_level = LogLevel::Information;
LogStyle log_style = LogStyle::FileOnly;
#endif

bool parse_arguments(argument_manager& arguments);
void display_help(void);

// Simple logger implementation
class SimpleLogger {
private:
    LogLevel level_;
    LogStyle style_;
    std::wstring name_;
    bool active_ = false;

public:
    SimpleLogger() : level_(LogLevel::Information), style_(LogStyle::ConsoleOnly) {}
    
    void set_target_level(LogLevel level) { level_ = level; }
    void set_write_console(LogStyle style) { style_ = style; }
    
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
    
    Timer chrono_start() {
        return Timer();
    }
    
    void write(LogLevel msg_level, const std::wstring& message, Timer& timer) {
        if (!active_ || msg_level < level_) return;
        
        if (style_ != LogStyle::FileOnly) {
            std::wcout << get_level_prefix(msg_level) 
                     << L"[" << timer.elapsed_ms() << L"ms] "
                     << message << std::endl;
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

// Simple container class to demo the same functionality
class SimpleContainer {
private:
    std::unordered_map<std::wstring, std::wstring> values_;
    
public:
    SimpleContainer() {}
    
    void add(const std::wstring& name, const std::wstring& value) {
        values_[name] = value;
    }
    
    bool remove(const std::wstring& name) {
        return values_.erase(name) > 0;
    }
    
    std::wstring serialize() const {
        std::wstring result;
        for (const auto& [key, value] : values_) {
            result += L"[" + key + L"] = " + value + L"\n";
        }
        return result;
    }
    
    std::wstring to_xml() const {
        std::wstring result = L"<container>\n";
        for (const auto& [key, value] : values_) {
            result += L"  <item key=\"" + key + L"\">" + value + L"</item>\n";
        }
        result += L"</container>";
        return result;
    }
    
    std::wstring to_json() const {
        std::wstring result = L"{\n";
        bool first = true;
        for (const auto& [key, value] : values_) {
            if (!first) result += L",\n";
            result += L"  \"" + key + L"\": \"" + value + L"\"";
            first = false;
        }
        result += L"\n}";
        return result;
    }
};

int main(int argc, char* argv[])
{
    std::wcout << L"Container sample starting..." << std::endl;
    
    // Set default values first
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
            std::wcout << L"Argument parsing returned early (likely help was displayed)" << std::endl;
            return 0;
        }
    } else {
        std::wcout << L"No arguments provided, using defaults" << std::endl;
    }

    std::wcout << L"Starting logger..." << std::endl;
    SimpleLogger logger;
    logger.set_target_level(log_level);
    logger.set_write_console(LogStyle::ConsoleOnly); // Force console output for testing
    logger.start(PROGRAM_NAME);

    auto start = logger.chrono_start();
    SimpleContainer data;
    data.add(L"false_value", L"false");
    data.add(L"true_value", L"true");
    data.add(L"float_value", L"1.234567");
    data.add(L"double_value", L"1.234567890123456789");
    
    logger.write(LogLevel::Information, 
                fmt::format(L"data serialize:\n{}", data.serialize()), start);
    logger.write(LogLevel::Information,
                fmt::format(L"data xml:\n{}", data.to_xml()), start);
    logger.write(LogLevel::Information,
                fmt::format(L"data json:\n{}", data.to_json()), start);

    start = logger.chrono_start();
    SimpleContainer data2;
    data2.add(L"false_value", L"false");
    data2.add(L"true_value", L"true");
    data2.add(L"float_value", L"1.234567");
    data2.add(L"double_value", L"1.234567890123456789");
    data2.add(L"long_value", std::to_wstring(LONG_MAX));
    data2.add(L"ulong_value", std::to_wstring(ULONG_MAX));
    data2.add(L"llong_value", std::to_wstring(LLONG_MAX));
    data2.add(L"ullong_value", std::to_wstring(ULLONG_MAX));
    data2.add(L"container_value", L"nested container example");
            
    logger.write(LogLevel::Information,
              fmt::format(L"data serialize:\n{}", data2.serialize()), start);
    logger.write(LogLevel::Information,
              fmt::format(L"data xml:\n{}", data2.to_xml()), start);
    logger.write(LogLevel::Information,
              fmt::format(L"data json:\n{}", data2.to_json()), start);

    start = logger.chrono_start();
    SimpleContainer data3 = data2;
    data3.remove(L"false_value");
    data3.remove(L"true_value");
    data3.remove(L"float_value");
    data3.remove(L"double_value");
    data3.remove(L"container_value");
    
    logger.write(LogLevel::Information,
              fmt::format(L"data serialize:\n{}", data3.serialize()), start);
    logger.write(LogLevel::Information,
              fmt::format(L"data xml:\n{}", data3.to_xml()), start);
    logger.write(LogLevel::Information,
              fmt::format(L"data json:\n{}", data3.to_json()), start);

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
    std::wcout << L"Container sample options:" << std::endl << std::endl;
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