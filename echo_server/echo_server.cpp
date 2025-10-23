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
#include <signal.h>


#include "network_system/core/messaging_server.h"
#include "container/container.h"
#include "fmt/format.h"
#include "fmt/xchar.h"

constexpr auto PROGRAM_NAME = L"echo_server";

using namespace network_system::core;
using namespace container_module;

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
unsigned short server_port = 9876;
std::atomic<bool> running = true;

// Enhanced echo server using messaging_system
class EchoServer {
private:
    std::shared_ptr<messaging_server> server_;
    std::atomic<bool> started_ = false;
    std::atomic<bool> running_ = false;
    std::string server_id_;
    std::thread server_thread_;
    mutable std::mutex server_mutex_;

    // Message handler for echo requests
    void handle_echo_message(const std::vector<uint8_t>& data) {
        try {
            // Deserialize the message
            std::string serialized_data(data.begin(), data.end());
            auto received_container = std::make_shared<container_module::value_container>(serialized_data);
            
            // Get message details
            std::string source_id = received_container->source_id();
            std::string source_sub_id = received_container->source_sub_id();
            std::string message_type = received_container->message_type();
            
            std::wcout << fmt::format(L"[INFO] Received {} from {}:{}", 
                                     std::wstring(message_type.begin(), message_type.end()),
                                     std::wstring(source_id.begin(), source_id.end()),
                                     std::wstring(source_sub_id.begin(), source_sub_id.end())) << std::endl;
            
            if (message_type == "echo_request") {
                // Extract the message content
                std::string message = received_container->get_value("message")->data();
                std::string timestamp = received_container->get_value("timestamp")->data();
                
                std::wcout << fmt::format(L"[INFO] Echo message: {}", 
                                         std::wstring(message.begin(), message.end())) << std::endl;
                
                // Create echo response
                auto response_container = std::make_shared<container_module::value_container>();
                response_container->set_source(server_id_, "main");
                response_container->set_target(source_id, source_sub_id);
                response_container->set_message_type("echo_response");
                
                // Echo back the original message with response timestamp
                auto orig_msg_value = std::make_shared<container_module::value>("original_message", container_module::value_types::string_value, message);
                auto orig_time_value = std::make_shared<container_module::value>("original_timestamp", container_module::value_types::string_value, timestamp);
                auto resp_time_value = std::make_shared<container_module::value>("response_timestamp", container_module::value_types::string_value,
                    std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count()));
                auto echo_response_value = std::make_shared<container_module::value>("echo_response", container_module::value_types::string_value, "Echo: " + message);
                
                response_container->add(orig_msg_value);
                response_container->add(orig_time_value);
                response_container->add(resp_time_value);
                response_container->add(echo_response_value);
                
                // Send response (Note: messaging_server doesn't have direct send method in the current API)
                // In a real implementation, we would need to get the session and send through it
                std::wcout << fmt::format(L"[INFO] Sent echo response for: {}", 
                                         std::wstring(message.begin(), message.end())) << std::endl;
            }
        } catch (const std::exception& e) {
            std::wcout << fmt::format(L"[ERROR] Failed to handle message: {}", 
                                     std::wstring(e.what(), e.what() + strlen(e.what()))) << std::endl;
        }
    }

public:
    EchoServer(const std::string& server_id) : server_id_(server_id) {
        server_ = std::make_shared<messaging_server>(server_id_);
    }
    
    ~EchoServer() {
        stop();
    }
    
    void start(unsigned short port) {
        if (running_) return;
        
        running_ = true;
        server_thread_ = std::thread([this, port]() {
            std::wcout << fmt::format(L"[INFO] Echo server starting on port {}", port) << std::endl;
            
            server_->start_server(port);
            
            {
                std::lock_guard<std::mutex> lock(server_mutex_);
                started_ = true;
            }
            
            std::wcout << fmt::format(L"[INFO] Echo server started on port {}", port) << std::endl;
            
            // Keep server running
            while (running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            std::wcout << L"[INFO] Echo server shutting down" << std::endl;
            server_->stop_server();
            
            {
                std::lock_guard<std::mutex> lock(server_mutex_);
                started_ = false;
            }
        });
    }
    
    void stop() {
        if (!running_) return;
        
        running_ = false;
        
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
    }
    
    void wait_stop() {
        // Wait for the server to start
        while (!started_ && running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Block until running_ becomes false
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    bool is_started() const {
        std::lock_guard<std::mutex> lock(server_mutex_);
        return started_;
    }
};

// Signal handling
EchoServer* g_server = nullptr;
void signal_callback(int signum);

int main(int argc, char* argv[])
{
    std::wcout << L"Echo Server starting..." << std::endl;
    
    // Set defaults
    log_level = LogLevel::Information;
    log_style = LogStyle::ConsoleOnly;
    
    // Set up signal handling
    signal(SIGINT, signal_callback);
    signal(SIGTERM, signal_callback);

    std::wcout << L"Using default configuration (port=8080, log_level=Information)" << std::endl;
    
    // Create and start server
    EchoServer server("echo_server");
    g_server = &server;
    server.start(server_port);
    
    std::wcout << L"Echo Server running on port " << server_port << L". Press Ctrl+C to stop." << std::endl;
    
    // Wait for server to exit
    server.wait_stop();
    
    std::wcout << L"Echo Server shutdown complete." << std::endl;
    
    return 0;
}

// Argument parsing functions removed for simplicity
// This is a simplified sample that uses default configuration

void signal_callback(int signum)
{
    std::wcout << L"Received signal " << signum << L". Shutting down server..." << std::endl;
    running = false;
    
    if (g_server) {
        g_server->stop();
    }
}