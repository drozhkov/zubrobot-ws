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
