#pragma once

// stl
#include <functional>

// boost
#include <boost/date_time.hpp>

// types
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;

typedef boost::posix_time::ptime timepoint_t;

// callback types
#if 0
typedef boost::function<void(int, const char*)> on_error_callback_t;
typedef boost::function<void(int64_t)> on_connected_callback_t;
typedef boost::function<void(int64_t)> on_disconnected_callback_t;
typedef boost::function<void(int64_t, uint8_t*, size_t)> on_received_data_callback_t;
#else
typedef std::function<void(int, const char*)> on_error_callback_t;
typedef std::function<void(int64_t)> on_connected_callback_t;
typedef std::function<void(int64_t)> on_disconnected_callback_t;
typedef std::function<void(int64_t, uint8_t*, size_t)> on_received_data_callback_t;
#endif
