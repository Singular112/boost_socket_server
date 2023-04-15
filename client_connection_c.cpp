#include "stdafx.h"
#include "client_connection_c.h"

//

client_connection_c::client_connection_c(boost::asio::io_service& io_service_ref,
	const on_received_data_callback_t& on_received_data_callback,
	const on_error_callback_t& on_error_callback,
	const on_disconnected_callback_t& on_disconnected_callback)
	: m_socket(io_service_ref)
	, m_strand(io_service_ref)
	, m_on_received_data_callback(on_received_data_callback)
	, m_on_error_callback(on_error_callback)
	, m_on_disconnected_callback(on_disconnected_callback)
{
}


client_connection_c::~client_connection_c()
{
}


void client_connection_c::initialize_after_connect()
{
	m_connection_opened_tp = boost::posix_time::microsec_clock::universal_time();
}


void client_connection_c::start_async_read()
{
	m_socket.async_read_some
	(
		boost::asio::buffer(m_static_data_buffer, m_static_data_buffer.size()),

		m_strand.wrap
		(
			boost::bind
			(
				&client_connection_c::handle_read,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		)
	);
}


void client_connection_c::handle_read(const boost::system::error_code& err,
	std::size_t bytes_received)
{
	if (!err)
	{
		m_on_received_data_callback
		(
			m_id,
			(uint8_t*)m_static_data_buffer.data(), bytes_received
		);

		start_async_read();
	}
	else
	{
		boost::system::error_code temp_err_code;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, temp_err_code);

		m_on_error_callback(err.value(), err.message().c_str());

		// report server client socket close
		m_on_disconnected_callback(m_id);
	}
}


void client_connection_c::async_write_data(uint8_t* data, size_t data_size)
{
	boost::asio::async_write
	(
		m_socket,
		boost::asio::buffer(data, data_size),

		m_strand.wrap
		(
			boost::bind(&client_connection_c::handle_write,
				shared_from_this(),
				boost::asio::placeholders::error)
		)
	);
}


void client_connection_c::handle_write(const boost::system::error_code& err)
{
	if (err)
	{
		boost::system::error_code temp_err_code;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, temp_err_code);

		// report server client socket close
		m_on_disconnected_callback(m_id);
	}
}


bool client_connection_c::write_data(uint8_t* data,
	size_t data_size,
	boost::system::error_code& err)
{
	// todo: use strand

	auto bytes_tranfered = boost::asio::write
	(
		m_socket,
		boost::asio::buffer(data, data_size),
		boost::asio::transfer_all(),
		err
	);

	return !err && bytes_tranfered == data_size;
}
