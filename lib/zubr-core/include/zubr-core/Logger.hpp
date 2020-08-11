/*
MIT License

Copyright (c) 2020 Denis Rozhkov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

#define ZUBR_LOG_DEBUG( m ) BOOST_LOG_TRIVIAL( debug ) << __func__ << ":: " << m
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
