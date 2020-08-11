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

/// Types.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_TYPES__H
#define __ZUBR_CONNECTOR_WS_TYPES__H


#include <chrono>
#include <cmath>

#include "zubr-core/Serializer.hpp"
#include "zubr-core/Types.hpp"


namespace zubr {

	typedef int64_t t_req_id;


	enum class Channel {
		_undef = 0,
		Instruments,
		Orders,
		OrderFills,
		LastTrades,
		Positions,
		OrderBook,
		Balance,
		Candles,
		RiskSettings,
		Tickers
	};

	class ChannelEnumHelper {
	public:
		static const char * ToString( Channel channel )
		{
			switch ( channel ) {
				case Channel::Instruments:
					return "instruments";

				case Channel::OrderBook:
					return "orderbook";

				case Channel::Orders:
					return "orders";

				case Channel::Positions:
					return "positions";

				default:
					return "N/A";
			}
		}

		static Channel FromChannelName( const std::string & name )
		{
			if ( ToString( Channel::Instruments ) == name ) {
				return Channel::Instruments;
			}

			if ( ToString( Channel::OrderBook ) == name ) {
				return Channel::OrderBook;
			}

			if ( ToString( Channel::Orders ) == name ) {
				return Channel::Orders;
			}

			if ( ToString( Channel::Positions ) == name ) {
				return Channel::Positions;
			}

			return Channel::_undef;
		}
	};

} // namespace zubr


#endif
