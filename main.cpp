#include "tcp_server_c.h"

// stl
#include <mutex>
#include <thread>
#include <stdio.h>
#include <iostream>

#ifdef WIN32
#	include <conio.h>

#	define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#	include <windows.h>

#	ifdef _M_X64
#		ifdef _DEBUG
#			pragma comment(lib, "bin/SockServer64d.lib")
#		else
#			pragma comment(lib, "bin/SockServer64.lib")
#		endif
#	else
#		ifdef _DEBUG
#			pragma comment(lib, "bin/SockServer32d.lib")
#		else
#			pragma comment(lib, "bin/SockServer32.lib")
#		endif
#	endif
#else
#	include <unistd.h>
#endif

std::mutex g_io_mtx;

//

class real_tcp_server_c
	: public tcp_server_c
{
public:
	void on_error_callback(int errcode, const char* text)
	{
		std::unique_lock<std::mutex> guard(g_io_mtx);

		if (errcode != 995)	// failed accept
			printf("%d, %s\n", errcode, text);
	}

	void on_connected_callback(int64_t connection_id)
	{
		std::unique_lock<std::mutex> guard(g_io_mtx);

		printf("New client connected: %d\n", connection_id);
	}

	void on_disconnected_callback(int64_t connection_id)
	{
		std::unique_lock<std::mutex> guard(g_io_mtx);

		std::cout << "Client disconnected: " << connection_id << std::endl;
	}

	void on_received_data_callback(int64_t connection_id, uint8_t* data, size_t data_size)
	{
		{
			std::unique_lock<std::mutex> guard(g_io_mtx);

			std::string string_data(data, data + data_size);
			printf("[%d] received data: %s\n",
#ifdef WIN32
				::GetCurrentThreadId(),
#else
				0,
#endif
				string_data.c_str());
		}

		bool result = write_data(connection_id, (uint8_t*)"answer", 8);
		if (!result)
		{
			std::unique_lock<std::mutex> guard(g_io_mtx);

			printf("Failed to send answer");
		}
	}
};

//

#ifdef WIN32
#	define usleep(x) Sleep((x) / 1000)
#endif

int main(int argc, char* argv[])
{
#if 1

	real_tcp_server_c server;
	server.start(27015, 4);

#elif 0

	tcp_server_c server;
	server.set_on_connected_callback([](int64_t connection_id)
	{
		std::unique_lock<std::mutex> guard(g_io_mtx);

		std::cout << "New client connected: " << connection_id << std::endl;
	});

	server.set_on_disconnected_callback([](int64_t connection_id)
	{
		std::unique_lock<std::mutex> guard(g_io_mtx);

		std::cout << "Client disconnected: " << connection_id << std::endl;
	});

	server.set_on_received_data_callback([&](int64_t connection_id, uint8_t* data, size_t data_size)
	{
		{
			std::unique_lock<std::mutex> guard(g_io_mtx);

			std::string string_data(data, data + data_size);
			printf("[%d] received data: %s\n",
#ifdef WIN32
				::GetCurrentThreadId(),
#else
				0,
#endif
				, string_data.c_str());
		}

		bool result = server.write_data(connection_id, (uint8_t*)"answer", 8);
		if (!result)
		{
			std::unique_lock<std::mutex> guard(g_io_mtx);

			printf("Failed to send answer");
		}
	});

	server.set_on_error_callback([](int errcode, const char* text)
	{
		std::unique_lock<std::mutex> guard(g_io_mtx);

		if (code != 995)	// failed accept
			printf("error: %d, %s\n", code, text);
	});

	server.start(27015, 8);

#endif

	while (true)
	{
		usleep(1000 * 500);
	}

	return 0;
}
