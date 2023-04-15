#pragma once

/*
	examples:

	======================================================
	//						one way						//

	#include "tcp_server_c.h"

	class real_tcp_server_c
		: public tcp_server_c
	{
	public:
		void on_error_callback(int errcode, const char* text)
		{
			printf("%d, %s\n", errcode, text);
		}

		void on_connected_callback(int64_t connection_id)
		{
			printf("New client connected: %d\n", connection_id);
		}

		void on_disconnected_callback(int64_t connection_id)
		{
			std::cout << "Client disconnected: " << connection_id << std::endl;
		}

		void on_received_data_callback(int64_t connection_id, uint8_t* data, size_t data_size)
		{
			std::string string_data(data, data + data_size);
			printf("[%d] received data: %s\n", ::GetCurrentThreadId(), string_data.c_str());

			bool result = write_data(connection_id, (uint8_t*)"answer", 8);
			if (!result)
			{
				printf("Failed to send answer");
			}
		}
	};

	int _tmain(int argc, _TCHAR* argv[])
	{
		real_tcp_server_c server;
		server.start(27015, 4);

		_getch();

		server.stop();

		return 0;
	}

	======================================================
	//					another way						//

	#include "tcp_server_c.h"

	int _tmain(int argc, _TCHAR* argv[])
	{
		tcp_server_c server;

		server.set_on_connected_callback([](int64_t connection_id)
		{
			std::cout << "New client connected: " << connection_id << std::endl;
		});

		server.set_on_disconnected_callback([](int64_t connection_id)
		{
			std::cout << "Client disconnected: " << connection_id << std::endl;
		});

		server.set_on_received_data_callback([&](int64_t connection_id, uint8_t* data, size_t data_size)
		{
			std::string string_data(data, data + data_size);
			printf("[%d] received data: %s\n", ::GetCurrentThreadId(), string_data.c_str());

			bool result = server.write_data(connection_id, (uint8_t*)"answer", 8);
			if (!result)
			{
				printf("Failed to send answer");
			}
		});

		server.set_on_error_callback([](int code, const char* text)
		{
			if (code != 995)	// failed accept
				printf("error: %d, %s\n", code, text);
		});

		server.start(27015, 8);

		_getch();

		server.stop();

		return 0;
	}
*/

// ==============================================================

#ifdef TCP_SERVER_LIB_EXPORTS
#	define TCP_SERVER_LIB_API __declspec(dllexport)
#else
#	define TCP_SERVER_LIB_API 
	//__declspec(dllimport)
#endif

// stl
#include <string>
#include <stdint.h>
#include <functional>

//

class TCP_SERVER_LIB_API tcp_server_c
{
public:
	tcp_server_c();
	virtual ~tcp_server_c();

	virtual bool start(uint16_t port, size_t thread_pool_size);

	virtual void stop();

	//
	virtual bool async_write_data(int64_t connection_id, uint8_t* data, size_t data_size);

	virtual void async_write_data_all(uint8_t* data, size_t data_size);

	virtual bool write_data(int64_t connection_id, uint8_t* data, size_t data_size);

	//
	int64_t port() const;

	bool started() const;

	size_t connection_count() const;

	size_t thread_pool_size() const;

	// callbacks - first way
	void set_on_error_callback(const std::function<void(int, const char*)>& func);
	void set_on_connected_callback(const std::function<void(int64_t)>& func);
	void set_on_disconnected_callback(const std::function<void(int64_t)>& func);
	void set_on_received_data_callback(const std::function<void(int64_t, uint8_t*, size_t)>& func);

	// callbacks - second way
	virtual void on_error_callback(int code, const char* text) {}
	virtual void on_connected_callback(int64_t connection_id) {};
	virtual void on_disconnected_callback(int64_t connection_id) {};
	virtual void on_received_data_callback(int64_t connection_id, uint8_t* data, size_t data_size) {};

private:
	class server_impl_c* m_d_ptr;
};
