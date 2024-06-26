﻿/*****************************************************************************
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
#include <stdlib.h>
#include <string>

#include "argument_parser.h"
#include "converting.h"
#include "file_handler.h"
#include "job.h"
#include "job_pool.h"
#include "logging.h"
#include "messaging_server.h"
#include "thread_pool.h"

#include "container.h"
#include "values/container_value.h"
#include "values/string_value.h"

#include "fmt/format.h"
#include "fmt/xchar.h"

#include <signal.h>

constexpr auto PROGRAM_NAME = L"echo_server";

using namespace std;
using namespace logging;
using namespace threads;
using namespace network;
using namespace converting;
using namespace file_handler;
using namespace argument_parser;

bool encrypt_mode = false;
bool compress_mode = false;
bool binary_mode = false;
unsigned short compress_block_size = 1024;
#ifdef _DEBUG
logging_level log_level = logging_level::parameter;
logging_styles logging_style = logging_styles::console_only;
#else
logging_level log_level = logging_level::information;
logging_styles logging_style = logging_styles::file_only;
#endif
wstring connection_key = L"echo_network";
unsigned short server_port = 9876;
unsigned short high_priority_count = 4;
unsigned short normal_priority_count = 4;
unsigned short low_priority_count = 4;
size_t session_limit_count = 0;

shared_ptr<thread_pool> _thread_pool = nullptr;

map<wstring, function<void(const vector<uint8_t>&)>> _registered_messages;

shared_ptr<messaging_server> _server = nullptr;

bool parse_arguments(argument_manager& arguments);
void display_help(void);

void create_server(void);
void create_thread_pool(void);
void connection(const wstring& target_id,
				const wstring& target_sub_id,
				const bool& condition);
void received_message(shared_ptr<container::value_container> container);
void received_binary_message(const wstring& source_id,
							 const wstring& source_sub_id,
							 const wstring& target_id,
							 const wstring& target_sub_id,
							 const vector<uint8_t>& data);
void received_echo_test(const vector<uint8_t>& data);
void signal_callback(int signum);

int main(int argc, char* argv[])
{
	argument_manager arguments(argc, argv);
	if (!parse_arguments(arguments))
	{
		return 0;
	}

	signal(SIGINT, signal_callback);
	signal(SIGILL, signal_callback);
	signal(SIGABRT, signal_callback);
	signal(SIGFPE, signal_callback);
	signal(SIGSEGV, signal_callback);
	signal(SIGTERM, signal_callback);

	logger::handle().set_write_console(logging_style);
	logger::handle().set_target_level(log_level);
#ifdef _WIN32
	logger::handle().start(PROGRAM_NAME, locale("ko_KR.UTF-8"));
#else
	logger::handle().start(PROGRAM_NAME);
#endif

	_registered_messages.insert({ L"echo_test", received_echo_test });

	create_thread_pool();

	create_server();

	_server->wait_stop();

	_thread_pool->stop();

	logger::handle().stop();

	return 0;
}

bool parse_arguments(argument_manager& arguments)
{
	wstring temp;

	auto string_target = arguments.to_string(L"--help");
	if (string_target != nullopt)
	{
		display_help();

		return false;
	}

	auto bool_target = arguments.to_bool(L"--encrypt_mode");
	if (bool_target != nullopt)
	{
		encrypt_mode = *bool_target;
	}

	bool_target = arguments.to_bool(L"--compress_mode");
	if (bool_target != nullopt)
	{
		compress_mode = *bool_target;
	}

	bool_target = arguments.to_bool(L"--binary_mode");
	if (bool_target != nullopt)
	{
		binary_mode = *bool_target;
	}

	auto ushort_target = arguments.to_ushort(L"--compress_block_size");
	if (ushort_target != nullopt)
	{
		compress_block_size = *ushort_target;
	}

	string_target = arguments.to_string(L"--connection_key");
	if (string_target != nullopt)
	{
		temp = converter::to_wstring(file::load(*string_target));
		if (!temp.empty())
		{
			connection_key = temp;
		}
	}

	ushort_target = arguments.to_ushort(L"--server_port");
	if (ushort_target != nullopt)
	{
		server_port = *ushort_target;
	}

	ushort_target = arguments.to_ushort(L"--high_priority_count");
	if (ushort_target != nullopt)
	{
		high_priority_count = *ushort_target;
	}

	ushort_target = arguments.to_ushort(L"--normal_priority_count");
	if (ushort_target != nullopt)
	{
		normal_priority_count = *ushort_target;
	}

	ushort_target = arguments.to_ushort(L"--low_priority_count");
	if (ushort_target != nullopt)
	{
		low_priority_count = *ushort_target;
	}

	auto int_target = arguments.to_int(L"--logging_level");
	if (int_target != nullopt)
	{
		log_level = (logging_level)*int_target;
	}

#ifdef _WIN32
	auto ullong_target = arguments.to_ullong(L"--session_limit_count");
	if (ullong_target != nullopt)
	{
		session_limit_count = *ullong_target;
	}
#else
	auto ulong_target = arguments.to_ulong(L"--session_limit_count");
	if (ulong_target != nullopt)
	{
		session_limit_count = *ulong_target;
	}
#endif

	bool_target = arguments.to_bool(L"--write_console_only");
	if (bool_target != nullopt && *bool_target)
	{
		logging_style = logging_styles::console_only;

		return true;
	}

	bool_target = arguments.to_bool(L"--write_console");
	if (bool_target != nullopt && *bool_target)
	{
		logging_style = logging_styles::file_and_console;

		return true;
	}

	logging_style = logging_styles::file_only;

	return true;
}

void display_help(void)
{
	wcout << L"pathfinder options:" << endl << endl;
	wcout << L"--encrypt_mode [value] " << endl;
	wcout << L"\tThe encrypt_mode on/off. If you want to use encrypt mode must "
			 L"be appended '--encrypt_mode true'.\n\tInitialize value is "
			 L"--encrypt_mode off."
		  << endl
		  << endl;
	wcout << L"--compress_mode [value]" << endl;
	wcout
		<< L"\tThe compress_mode on/off. If you want to use compress mode must "
		   L"be appended '--compress_mode true'.\n\tInitialize value is "
		   L"--compress_mode off."
		<< endl
		<< endl;
	wcout << L"--compress_block_size [value]" << endl;
	wcout
		<< L"\tThe compress_mode on/off. If you want to change compress block "
		   L"size must be appended '--compress_block_size size'.\n\tInitialize "
		   L"value is --compress_mode 1024."
		<< endl
		<< endl;
	wcout << L"--connection_key [value]" << endl;
	wcout
		<< L"\tIf you want to change a specific key string for the connection "
		   L"to the main server must be appended\n\t'--connection_key "
		   L"[specific key string]'."
		<< endl
		<< endl;
	wcout << L"--server_port [value]" << endl;
	wcout << L"\tIf you want to change a port number for the connection to the "
			 L"main server must be appended\n\t'--server_port [port number]'."
		  << endl
		  << endl;
	wcout << L"--high_priority_count [value]" << endl;
	wcout << L"\tIf you want to change high priority thread workers must be "
			 L"appended '--high_priority_count [count]'."
		  << endl
		  << endl;
	wcout << L"--normal_priority_count [value]" << endl;
	wcout << L"\tIf you want to change normal priority thread workers must be "
			 L"appended '--normal_priority_count [count]'."
		  << endl
		  << endl;
	wcout << L"--low_priority_count [value]" << endl;
	wcout << L"\tIf you want to change low priority thread workers must be "
			 L"appended '--low_priority_count [count]'."
		  << endl
		  << endl;
	wcout << L"--session_limit_count [value]" << endl;
	wcout << L"\tIf you want to change session limit count must be appended "
			 L"'--session_limit_count [count]'."
		  << endl
		  << endl;
	wcout << L"--write_console [value] " << endl;
	wcout << L"\tThe write_console_mode on/off. If you want to display log on "
			 L"console must be appended '--write_console true'.\n\tInitialize "
			 L"value is --write_console off."
		  << endl
		  << endl;
	wcout << L"--logging_level [value]" << endl;
	wcout << L"\tIf you want to change log level must be appended "
			 L"'--logging_level [level]'."
		  << endl;
}

void create_server(void)
{
	if (_server != nullptr)
	{
		_server.reset();
	}

	_server = make_shared<messaging_server>(PROGRAM_NAME);
	_server->set_encrypt_mode(encrypt_mode);
	_server->set_compress_mode(compress_mode);
	_server->set_connection_key(connection_key);
	_server->set_session_limit_count(session_limit_count);
	_server->set_connection_notification(&connection);
	if (binary_mode)
	{
		_server->set_binary_notification(&received_binary_message);
		_server->set_possible_session_types({ session_types::binary_line });
	}
	else
	{
		_server->set_message_notification(&received_message);
		_server->set_possible_session_types({ session_types::message_line });
	}
	_server->start(server_port, high_priority_count, normal_priority_count,
				   low_priority_count);
}

void create_thread_pool(void)
{
	if (_thread_pool != nullptr)
	{
		_thread_pool.reset();
	}

	_thread_pool = make_shared<thread_pool>();
	for (unsigned short high = 0; high < high_priority_count; ++high)
	{
		_thread_pool->append(make_shared<thread_worker>(priorities::high));
	}
	for (unsigned short normal = 0; normal < normal_priority_count; ++normal)
	{
		_thread_pool->append(make_shared<thread_worker>(
			priorities::normal, vector<priorities>{ priorities::high }));
	}
	for (unsigned short low = 0; low < low_priority_count; ++low)
	{
		_thread_pool->append(make_shared<thread_worker>(
			priorities::low,
			vector<priorities>{ priorities::high, priorities::normal }));
	}
	_thread_pool->start();
}

void connection(const wstring& target_id,
				const wstring& target_sub_id,
				const bool& condition)
{
	logger::handle().write(
		logging_level::information,
		fmt::format(L"an echo_client({}[{}]) is {} an echo_server", target_id,
					target_sub_id,
					condition ? L"connected to" : L"disconnected from"));
}

void received_message(shared_ptr<container::value_container> container)
{
	if (container == nullptr)
	{
		return;
	}

	auto message_type = _registered_messages.find(container->message_type());
	if (message_type != _registered_messages.end())
	{
		if (_thread_pool)
		{
			_thread_pool->push(make_shared<job>(
				priorities::high, converter::to_array(container->serialize()),
				message_type->second));
		}

		return;
	}

	logger::handle().write(
		logging_level::information,
		fmt::format(L"received message: {}", container->serialize()));
}

void received_binary_message(const wstring& source_id,
							 const wstring& source_sub_id,
							 const wstring& target_id,
							 const wstring& target_sub_id,
							 const vector<uint8_t>& data)
{
	logger::handle().write(logging_level::information,
						   fmt::format(L"received message: {}[{}] = {}",
									   source_id, source_sub_id,
									   converter::to_wstring(data)));

	_server->send_binary(source_id, source_sub_id, data);
}

void received_echo_test(const vector<uint8_t>& data)
{
	if (data.empty())
	{
		return;
	}

	shared_ptr<container::value_container> container
		= make_shared<container::value_container>(data, false);
	if (container == nullptr)
	{
		return;
	}

	logger::handle().write(
		logging_level::information,
		fmt::format(L"received message: {}", container->serialize()));

	shared_ptr<container::value_container> message = container->copy(false);
	message->swap_header();

	_server->send(message);
}

void signal_callback(int signum) { _server->stop(); }