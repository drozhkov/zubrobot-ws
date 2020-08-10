/// Logger.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_LOGGER__H
#define __ZUBR_LOGGER__H


#include "boost/log/core.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/trivial.hpp"


namespace zubr {

#define ZUBR_LOG_DEBUG( m ) BOOST_LOG_TRIVIAL( debug ) << m
#define ZUBR_LOG_INFO( m ) BOOST_LOG_TRIVIAL( info ) << m
#define ZUBR_LOG_ERROR( m ) BOOST_LOG_TRIVIAL( error ) << m

#define ZUBR_LOG_SET_LEVEL( l )                                                \
	boost::log::core::get()->set_filter(                                       \
		boost::log::trivial::severity                                          \
		>= ( zubr::LogLevel::Debug == l                                        \
				 ? boost::log::trivial::debug                                  \
				 : ( zubr::LogLevel::Info == l                                 \
						 ? boost::log::trivial::info                           \
						 : boost::log::trivial::error ) ) )


	enum class LogLevel { _undef = 0, Debug, Info, Error };

} // namespace zubr


#endif
