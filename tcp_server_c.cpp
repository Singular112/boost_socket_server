#include "stdafx.h"
#include "tcp_server_c.h"
#include "server_impl_c.h"

//

tcp_server_c::tcp_server_c()
{
	m_d_ptr = new server_impl_c();

	// setup defaults callbacks to virtual functions
	{
		m_d_ptr->set_on_error_callback
		(
			std::bind
			(
				&tcp_server_c::on_error_callback,
				this,
				std::placeholders::_1,
				std::placeholders::_2
			)
		);

		m_d_ptr->set_on_connected_callback
		(
			std::bind
			(
				&tcp_server_c::on_connected_callback,
				this, std::placeholders::_1
			)
		);

		m_d_ptr->set_on_disconnected_callback
		(
			std::bind
			(
				&tcp_server_c::on_disconnected_callback,
				this,
				std::placeholders::_1
			)
		);

		m_d_ptr->set_on_received_data_callback
		(
			std::bind
			(
				&tcp_server_c::on_received_data_callback,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			)
		);
	}
}


tcp_server_c::~tcp_server_c()
{
	delete m_d_ptr;
}


bool tcp_server_c::start(uint16_t port, size_t thread_pool_size)
{
	return m_d_ptr->start(port, thread_pool_size);
}


void tcp_server_c::stop()
{
	m_d_ptr->stop();
}


bool tcp_server_c::async_write_data(int64_t connection_id, uint8_t* data, size_t data_size)
{
	return m_d_ptr->async_write_data(connection_id, data, data_size);
}


void tcp_server_c::async_write_data_all(uint8_t* data, size_t data_size)
{
	m_d_ptr->async_write_data_all(data, data_size);
}


bool tcp_server_c::write_data(int64_t connection_id, uint8_t* data, size_t data_size)
{
	return m_d_ptr->write_data(connection_id, data, data_size);
}


int64_t tcp_server_c::port() const
{
	return m_d_ptr->port();
}


bool tcp_server_c::started() const
{
	return m_d_ptr->started();
}


size_t tcp_server_c::connection_count() const
{
	return m_d_ptr->connection_count();
}


size_t tcp_server_c::thread_pool_size() const
{
	return m_d_ptr->thread_pool_size();
}


void tcp_server_c::set_on_error_callback(const std::function<void(int, const char*)>& func)
{
	m_d_ptr->set_on_error_callback(func);
}


void tcp_server_c::set_on_connected_callback(const std::function<void(int64_t)>& func)
{
	m_d_ptr->set_on_connected_callback(func);
}


void tcp_server_c::set_on_disconnected_callback(const std::function<void(int64_t)>& func)
{
	m_d_ptr->set_on_disconnected_callback(func);
}


void tcp_server_c::set_on_received_data_callback(const std::function<void(int64_t, uint8_t*, size_t)>& func)
{
	m_d_ptr->set_on_received_data_callback(func);
}
