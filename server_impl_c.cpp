#include "stdafx.h"
#include "server_impl_c.h"

//

server_impl_c::server_impl_c()
{
}


server_impl_c::~server_impl_c()
{
	stop();
}


void server_impl_c::start_accept(client_connection_t& conn)
{
	m_acceptor->async_accept
	(
		conn->socket(),
		boost::bind(&server_impl_c::handle_accept, this, conn, boost::asio::placeholders::error)
	);
}


void server_impl_c::handle_connection(client_connection_t& conn)
{
	auto connection_id = m_inner_clientid_counter++;
	conn->set_id(connection_id);

	{
		boost::unique_lock<client_map_mtx_t> guard(m_client_map_mtx);
		m_client_map.emplace(connection_id, conn);
	}

	conn->initialize_after_connect();
}


void server_impl_c::inner_on_client_disconnected(int64_t connection_id)
{
	{
		boost::unique_lock<client_map_mtx_t> guard(m_client_map_mtx);
		m_client_map.erase(connection_id);
	}

	m_on_disconnected_callback(connection_id);
}


void server_impl_c::handle_accept(client_connection_t conn, const boost::system::error_code& err)
{
	if (err)
	{
		m_on_error_callback(err.value(), err.message().c_str());
		return;
	}
	
	handle_connection(conn);

	conn->start_async_read();

	m_on_connected_callback(conn->id());

	// continue accept
	auto new_conn = client_connection_c::new_shared_connection
	(
		m_io_service,
		m_on_received_data_callback,
		m_on_error_callback,

		std::bind(&server_impl_c::inner_on_client_disconnected, this, std::placeholders::_1)
	);

	start_accept(new_conn);
}


bool server_impl_c::start(uint16_t port, size_t thread_pool_size)
{
	if (started())
		return false;

	m_tcp_port = port;
	m_thread_pool_size = thread_pool_size;

	try
	{
#if (BOOST_VERSION <= 106600)	// < 1.0.66
		if (m_io_service.stopped())
			m_io_service.reset();
#else
		if (m_io_service.stopped())
			m_io_service.restart();
#endif

		ip::tcp::endpoint ep(ip::tcp::v4(), port);
		m_acceptor.reset(new ip::tcp::acceptor(m_io_service, ep));
		m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		m_acceptor->listen();

		auto new_conn = client_connection_c::new_shared_connection
		(
			m_io_service,
			m_on_received_data_callback,
			m_on_error_callback,

			std::bind(&server_impl_c::inner_on_client_disconnected, this, std::placeholders::_1)
		);

		start_accept(new_conn);

		// initialize thread_pool
		m_thread_pool.reset(new boost::thread_group);

		for (size_t i = 0; i < thread_pool_size; ++i)
		{
			m_thread_pool->create_thread
			(
				boost::bind(&boost::asio::io_service::run, &m_io_service)
			);
		}

		m_started = true;

		return true;
	}
	catch (const boost::system::system_error&)
	{
	}

	return false;
}


void server_impl_c::stop()
{
	m_acceptor.reset();

	if (!m_io_service.stopped())
		m_io_service.stop();

	// finalize workers
	if (m_thread_pool)
	{
		m_thread_pool->interrupt_all();
		m_thread_pool->join_all();
		m_thread_pool.reset();
	}

	{
		boost::unique_lock<client_map_mtx_t> guard(m_client_map_mtx);
		m_client_map.clear();
	}

	m_started = false;
}


bool server_impl_c::async_write_data(int64_t connection_id, uint8_t* data, size_t data_size)
{
	boost::shared_lock<client_map_mtx_t> guard(m_client_map_mtx);

	auto client_it = m_client_map.find(connection_id);
	if (client_it == m_client_map.cend())
	{
		return false;
	}

	client_connection_t& client_ptr = client_it->second;
	client_ptr->async_write_data(data, data_size);

	return true;
}


void server_impl_c::async_write_data_all(uint8_t* data, size_t data_size)
{
	boost::shared_lock<client_map_mtx_t> guard(m_client_map_mtx);
	for (auto& it : m_client_map)
	{
		it.second->async_write_data(data, data_size);
	}
}


bool server_impl_c::write_data(int64_t connection_id, uint8_t* data, size_t data_size)
{
	boost::shared_lock<client_map_mtx_t> guard(m_client_map_mtx);

	auto client_it = m_client_map.find(connection_id);
	if (client_it == m_client_map.cend())
	{
		return false;
	}

	boost::system::error_code err;

	client_connection_t& client_ptr = client_it->second;
	return client_ptr->write_data(data, data_size, err);
}


void server_impl_c::set_on_error_callback(const on_error_callback_t& func)
{
	m_on_error_callback = func;
}


void server_impl_c::set_on_connected_callback(const on_connected_callback_t& func)
{
	m_on_connected_callback = func;
}


void server_impl_c::set_on_disconnected_callback(const on_disconnected_callback_t& func)
{
	m_on_disconnected_callback = func;
}


void server_impl_c::set_on_received_data_callback(const on_received_data_callback_t& func)
{
	m_on_received_data_callback = func;
}
