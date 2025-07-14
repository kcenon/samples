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
#include <chrono>
#include <unordered_map>
#include <functional>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "utilities/parsing/argument_parser.h"
#include "container/container.h"
#include "network/network.h"

#include "fmt/format.h"
#include "fmt/xchar.h"

constexpr auto PROGRAM_NAME = L"echo_client";

using namespace utility_module;
using namespace container_module;
using namespace network_module;

// Simple logger
enum class LogLevel { Debug, Information, Warning, Error, Parameter };
enum class LogStyle { ConsoleOnly, FileOnly, FileAndConsole };

#ifdef _DEBUG
LogLevel log_level = LogLevel::Parameter;
LogStyle log_style = LogStyle::ConsoleOnly;
#else
LogLevel log_level = LogLevel::Information;
LogStyle log_style = LogStyle::FileOnly;
#endif

// Configuration
std::wstring server_ip = L"127.0.0.1";
unsigned short server_port = 9876;
bool running = true;

// Enhanced network client using messaging_system
class EchoClient {
private:
    std::shared_ptr<messaging_client> client_;
    std::atomic<bool> connected_ = false;
    std::atomic<bool> running_ = false;
    std::string client_id_;
    std::mutex message_mutex_;

public:
    EchoClient(const std::string& client_id) : client_id_(client_id) {
        client_ = std::make_shared<messaging_client>(client_id_);
    }
    
    ~EchoClient() {
        stop();
    }
    
    void start(const std::string& server_ip, unsigned short server_port) {
        if (running_) return;
        
        running_ = true;
        std::wcout << fmt::format(L"[INFO] Starting client and connecting to {}:{}", 
                                 std::wstring(server_ip.begin(), server_ip.end()), server_port) << std::endl;
        
        client_->start_client(server_ip, server_port);
        
        // Wait a moment for connection
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        connected_ = true;
        
        std::wcout << fmt::format(L"[INFO] Connected to {}:{}", 
                                 std::wstring(server_ip.begin(), server_ip.end()), server_port) << std::endl;
    }
    
    void stop() {
        if (!running_) return;
        
        running_ = false;
        connected_ = false;
        
        if (client_) {
            client_->stop_client();
        }
        
        std::wcout << L"[INFO] Client stopped" << std::endl;
    }
    
    bool is_connected() const {
        return connected_ && running_;
    }
    
    void send_echo_message(const std::string& message) {
        if (!is_connected()) {
            std::wcout << L"[WARNING] Not connected, cannot send message" << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(message_mutex_);
        
        // Create container with echo message
        auto msg_container = std::make_shared<container_module::value_container>();
        msg_container->set_source(client_id_, "echo_client");
        msg_container->set_target("echo_server", "main");
        msg_container->set_message_type("echo_request");
        
        // Add message content
        auto message_value = std::make_shared<container_module::value>("message", container_module::value_types::string_value, message);
        auto timestamp_value = std::make_shared<container_module::value>("timestamp", container_module::value_types::string_value, 
            std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()));
        
        msg_container->add(message_value);
        msg_container->add(timestamp_value);
        
        // Serialize and send
        std::string serialized = msg_container->serialize();
        std::vector<uint8_t> data(serialized.begin(), serialized.end());
        
        client_->send_packet(data);
        
        std::wcout << fmt::format(L"[INFO] Sent echo message: {}", 
                                 std::wstring(message.begin(), message.end())) << std::endl;
    }
};

// Argument parsing
bool parse_arguments(argument_manager& arguments);
void display_help(void);

int main(int argc, char* argv[])
{
    std::wcout << L"Echo Client starting..." << std::endl;
    
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
    
    // Create and start client
    EchoClient client("echo_client_001");
    
    // Convert wstring to string for the client
    std::string server_ip_str(server_ip.begin(), server_ip.end());
    client.start(server_ip_str, server_port);
    
    // Check connection
    if (!client.is_connected()) {
        std::wcout << L"Failed to connect to server" << std::endl;
        return 1;
    }
    
    // Main message loop - sends an echo message every 2 seconds
    int messageCount = 0;
    while (running && messageCount < 5) {
        std::string message = fmt::format("Echo test message #{}", messageCount++);
        client.send_echo_message(message);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    std::wcout << L"Echo Client shutting down..." << std::endl;
    client.stop();
    
    return 0;
}

bool parse_arguments(argument_manager& arguments)
{
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
    
    string_target = arguments.to_string("--server_ip");
    if (string_target.has_value())
    {
        server_ip = std::wstring(string_target.value().begin(), string_target.value().end());
    }
    
    auto ushort_target = arguments.to_ushort("--server_port");
    if (ushort_target.has_value())
    {
        server_port = ushort_target.value();
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
    std::wcout << L"Echo Client options:" << std::endl << std::endl;
    std::wcout << L"--server_ip [value]" << std::endl;
    std::wcout << L"\tSpecify the server IP address. Default is 127.0.0.1" << std::endl << std::endl;
    std::wcout << L"--server_port [value]" << std::endl;
    std::wcout << L"\tSpecify the server port. Default is 9876" << std::endl << std::endl;
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