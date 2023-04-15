#pragma once

// boost
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

// types
#include "common_types.h"

class client_connection_c;
typedef boost::shared_ptr<client_connection_c> client_connection_t;

// global constants
const int g_static_databuffer_size = 4096;

//

class client_connection_c
	: private boost::noncopyable
	, public boost::enable_shared_from_this<client_connection_c>
{
	friend class server_impl_c;

public:
	virtual ~client_connection_c();

	static client_connection_t new_shared_connection(boost::asio::io_service& io_service_ref,
		const on_received_data_callback_t& on_received_data_callback,
		const on_error_callback_t& on_error_callback,
		const on_disconnected_callback_t& on_disconnected_callback)
	{
		return client_connection_t
		(
			new client_connection_c
			(
				io_service_ref,
				on_received_data_callback,
				on_error_callback,
				on_disconnected_callback
			)
		);
	}

protected:
	client_connection_c(boost::asio::io_service& io_service_ref,
		const on_received_data_callback_t& on_received_data_callback,
		const on_error_callback_t& on_error_callback,
		const on_disconnected_callback_t& on_disconnected_callback);

	void initialize_after_connect();

	void start_async_read();

	void handle_read(const boost::system::error_code& err,
		std::size_t bytes_received);

	void async_write_data(uint8_t* data, size_t data_size);

	void handle_write(const boost::system::error_code& err);

	bool write_data(uint8_t* data,
		size_t data_size,
		boost::system::error_code& err);

	void set_id(int64_t id) { m_id = id; }

	int64_t id() const { return m_id; }

	boost::asio::ip::tcp::socket& socket() { return m_socket; }

private:
	boost::asio::ip::tcp::socket m_socket;

	boost::asio::io_service::strand m_strand;

	timepoint_t m_connection_opened_tp;

	int64_t m_id;

	boost::array<char, g_static_databuffer_size> m_static_data_buffer;

	// callbacks
	const on_received_data_callback_t& m_on_received_data_callback;
	const on_error_callback_t& m_on_error_callback;
	on_disconnected_callback_t m_on_disconnected_callback;
};
