#pragma once

// stl
#include <map>
#include <atomic>
#include <string>
#include <stdint.h>

// boost
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

//
#include "common_types.h"
#include "client_connection_c.h"

// global constants
const int g_default_threadpool_limit = 1;

//
using namespace boost::asio;

//

class server_impl_c
	: private boost::noncopyable
{
	typedef boost::scoped_ptr<ip::tcp::acceptor> acceptor_ptr_t;
	typedef boost::shared_mutex client_map_mtx_t;

public:
	server_impl_c();
	virtual ~server_impl_c();

	virtual bool start(uint16_t port,
		size_t thread_pool_size = g_default_threadpool_limit);

	virtual void stop();

	//
	virtual bool async_write_data(int64_t connection_id, uint8_t* data, size_t data_size);

	virtual void async_write_data_all(uint8_t* data, size_t data_size);

	virtual bool write_data(int64_t connection_id, uint8_t* data, size_t data_size);

	//
	uint16_t port() const { return m_tcp_port; }

	bool started() const { return m_started; }

	size_t connection_count() const { return m_client_map.size(); /* The C++11 standard guarantees that const method access to containers is safe from different threads */ };

	size_t thread_pool_size() const { return m_thread_pool_size; }

	// callbacks
	void set_on_error_callback(const on_error_callback_t& func);
	void set_on_connected_callback(const on_connected_callback_t& func);
	void set_on_disconnected_callback(const on_disconnected_callback_t& func);
	void set_on_received_data_callback(const on_received_data_callback_t& func);

protected:
	void start_accept(client_connection_t& conn);
	void handle_accept(client_connection_t conn,
		const boost::system::error_code & err);

	void handle_connection(client_connection_t& conn);

	void inner_on_client_disconnected(int64_t connection_id);

protected:
	uint16_t m_tcp_port = 0;

	boost::asio::io_service m_io_service;
	acceptor_ptr_t m_acceptor;

	std::map<int64_t, client_connection_t> m_client_map;
	client_map_mtx_t m_client_map_mtx;

	boost::scoped_ptr<boost::thread_group> m_thread_pool;
	size_t m_thread_pool_size = 0;

	//std::atomic_int64_t m_inner_clientid_counter;
	std::atomic<int64_t> m_inner_clientid_counter;

	bool m_started = false;

	// callbacks
	on_error_callback_t m_on_error_callback = [](int, const char*){};
	on_connected_callback_t m_on_connected_callback = [](int64_t){};
	on_disconnected_callback_t m_on_disconnected_callback = [](int64_t){};
	on_received_data_callback_t m_on_received_data_callback = [](int64_t, uint8_t*, size_t){};
};
