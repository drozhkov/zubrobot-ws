/// Types.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_TYPES__H
#define __ZUBR_CONNECTOR_WS_TYPES__H


#include <chrono>
#include <cmath>


namespace zubr {

	enum class OrderDirection { _undef = 0, Buy, Sell };
	enum class OrderType { _undef = 0, Limit, PostOnly };
	enum class OrderLifetime { _undef = 0, Gtc, IoC, FoK };
	enum class OrderStatus {
		_undef = 0,
		New,
		Filled,
		Cancelled,
		PartiallyFilled
	};

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

	class OrderEnumHelper {
	public:
		static const char * ToString( OrderDirection direction )
		{
			switch ( direction ) {
				case OrderDirection::Buy:
					return "BUY";

				case OrderDirection::Sell:
					return "SELL";

				default:
					return "N/A";
			}
		}

		static const char * ToString( OrderType type )
		{
			switch ( type ) {
				case OrderType::Limit:
					return "LIMIT";

				case OrderType::PostOnly:
					return "POST_ONLY";

				default:
					return "N/A";
			}
		}

		static const char * ToString( OrderLifetime lifetime )
		{
			switch ( lifetime ) {
				case OrderLifetime::FoK:
					return "FOK";

				case OrderLifetime::Gtc:
					return "GTC";

				case OrderLifetime::IoC:
					return "IOC";

				default:
					return "N/A";
			}
		}

		static const char * ToString( OrderStatus status )
		{
			switch ( status ) {
				case OrderStatus::Cancelled:
					return "CANCELLED";

				case OrderStatus::Filled:
					return "FILLED";

				case OrderStatus::New:
					return "NEW";

				case OrderStatus::PartiallyFilled:
					return "PARTIALLY_FILLED";

				default:
					return "N/A";
			}
		}

		static OrderType FromOrderTypeName( const std::string & name )
		{
			if ( ToString( OrderType::Limit ) == name ) {
				return OrderType::Limit;
			}

			if ( ToString( OrderType::PostOnly ) == name ) {
				return OrderType::PostOnly;
			}

			return OrderType::_undef;
		}

		static OrderLifetime FromOrderLifetimeName( const std::string & name )
		{
			if ( ToString( OrderLifetime::FoK ) == name ) {
				return OrderLifetime::FoK;
			}

			if ( ToString( OrderLifetime::Gtc ) == name ) {
				return OrderLifetime::Gtc;
			}

			if ( ToString( OrderLifetime::IoC ) == name ) {
				return OrderLifetime::IoC;
			}

			return OrderLifetime::_undef;
		}

		static OrderDirection FromOrderDirectionName( const std::string & name )
		{
			if ( ToString( OrderDirection::Buy ) == name ) {
				return OrderDirection::Buy;
			}

			if ( ToString( OrderDirection::Sell ) == name ) {
				return OrderDirection::Sell;
			}

			return OrderDirection::_undef;
		}

		static OrderStatus FromOrderStatusName( const std::string & name )
		{
			if ( ToString( OrderStatus::Cancelled ) == name ) {
				return OrderStatus::Cancelled;
			}

			if ( ToString( OrderStatus::Filled ) == name ) {
				return OrderStatus::Filled;
			}

			if ( ToString( OrderStatus::New ) == name ) {
				return OrderStatus::New;
			}

			if ( ToString( OrderStatus::PartiallyFilled ) == name ) {
				return OrderStatus::PartiallyFilled;
			}

			return OrderStatus::_undef;
		}
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

	struct SerializerWs;
	struct DeserializerWs;

	class Serializable {
	public:
		virtual void Serialize( SerializerWs * o )
		{
		}
	};

	class Deserializable {
	protected:
		static int64_t GetIntOrDefault( DeserializerWs * o,
			const std::string & memberName,
			int64_t defaultValue = INT64_C( -1 ) );

		static std::string GetStringOrDefault( DeserializerWs * o,
			const std::string & memberName,
			const std::string & defaultValue = "" );

	public:
		virtual void Deserialize( DeserializerWs * o )
		{
		}
	};

	class Number : public Serializable, public Deserializable {
	protected:
		int64_t m_significand;
		int m_exponent;

		double m_factor;

	protected:
		void Exponent( int exponent )
		{
			m_exponent = exponent;
			m_factor = std::pow( 10, exponent );
		}

	public:
		Number( int64_t significand, int exponent )
			: m_significand( significand )
		{
			Exponent( exponent );
		}

		Number()
			: Number( INT64_MAX, 0 )
		{
		}

		void Serialize( SerializerWs * o ) override;
		void Deserialize( DeserializerWs * o ) override;

		int64_t Integer() const
		{
			return ( m_significand * m_factor );
		}

		int64_t Fraction() const
		{
			return ( m_significand - ( Integer() * m_factor ) );
		}

		double Value() const
		{
			return ( m_significand * m_factor );
		}

		bool HasValue() const
		{
			return ( m_significand != INT64_MAX );
		}

		Number & Add( const Number & n )
		{
			if ( n.m_exponent == m_exponent ) {
				m_significand += n.m_significand;
			}
			else {
				m_significand = ( Value() + n.Value() ) / m_factor;
			}

			return *this;
		}

		Number & Add( double a )
		{
			m_significand += a / m_factor;

			return *this;
		}

		Number & Div( double d )
		{
			m_significand /= d;

			return *this;
		}

		Number & Sub( double s )
		{
			m_significand -= s / m_factor;

			return *this;
		}
	};

	class Time : public Serializable, public Deserializable {
	protected:
		uint64_t m_seconds;
		uint64_t m_nanoseconds;

	public:
		Time( uint64_t seconds, uint64_t nanoseconds = UINT64_C( 0 ) )
			: m_seconds( seconds )
			, m_nanoseconds( nanoseconds )
		{
		}

		static Time Now()
		{
			return Time( std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch() )
							 .count() );
		}

		uint64_t Seconds() const
		{
			return m_seconds;
		}

		uint64_t Nanoseconds() const
		{
			return m_nanoseconds;
		}

		void Serialize( SerializerWs * o ) override;
		void Deserialize( DeserializerWs * o ) override;
	};

} // namespace zubr


#endif
