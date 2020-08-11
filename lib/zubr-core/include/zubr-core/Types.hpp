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

#ifndef __ZUBR_TYPES__H
#define __ZUBR_TYPES__H


#include <chrono>
#include <cmath>
#include <iostream>

#include "Serializer.hpp"


namespace zubr {

	typedef int64_t t_order_id;
	typedef int t_instrument_id;


	class Number : public Serializable {
	protected:
		int64_t m_significand;
		int m_exponent;

		int64_t m_factor;

	protected:
		void Exponent( int exponent )
		{
			m_exponent = exponent;
			m_factor = std::pow( 10, std::abs( exponent ) );
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

		void Serialize( Serializer & o ) override;
		void Deserialize( Serializer & o ) override;

		int64_t Integer() const
		{
			return ( m_exponent < 0 ? m_significand / m_factor
									: m_significand * m_factor );
		}

		int64_t Fraction() const
		{
			return ( m_significand
					 - ( ( m_exponent < 0 ? Integer() / m_factor
										  : Integer() * m_factor ) ) );
		}

		double Value() const
		{
			return ( m_exponent < 0
						 ? static_cast<double>( m_significand ) / m_factor
						 : static_cast<double>( m_significand ) * m_factor );
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
				m_significand
					= ( m_exponent < 0 ? ( Value() + n.Value() ) * m_factor
									   : ( Value() + n.Value() ) / m_factor );
			}

			return *this;
		}

		Number & Add( double a )
		{
			m_significand += ( m_exponent < 0 ? a * m_factor : a / m_factor );

			return *this;
		}

		Number & Div( double d )
		{
			m_significand /= d;

			return *this;
		}

		Number & Sub( double s )
		{
			m_significand -= ( m_exponent < 0 ? s * m_factor : s / m_factor );

			return *this;
		}

		Number & ModRing( const Number & m )
		{
			m_significand
				= ( m_exponent < 0
						? ( std::round( Value() / m.Value() ) * m.Value() )
							  * m_factor
						: ( std::round( Value() / m.Value() ) * m.Value() )
							  / m_factor );

			return *this;
		}

		bool operator<( const Number & r ) const
		{
			auto exponentDiff = m_exponent - r.m_exponent;

			auto s1 = m_significand;
			auto s2 = r.m_significand;

			if ( 0 == exponentDiff ) {
				return ( m_significand < r.m_significand );
			}
			else if ( exponentDiff > 0 ) {
				s1 *= std::pow( 10, exponentDiff );
			}
			else if ( exponentDiff < 0 ) {
				s2 *= std::pow( 10, -exponentDiff );
			}

			return ( s1 < s2 );
		}
	};

	class Time : public Serializable {
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

		void Serialize( Serializer & o ) override;
		void Deserialize( Serializer & o ) override;
	};

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

	class OrderBookEntryItem : public Serializable {
	protected:
		Number m_price;
		int m_quantity;

	public:
		void Deserialize( Serializer & s ) override;

		const Number & Price() const
		{
			return m_price;
		}

		int Quantity() const
		{
			return m_quantity;
		}
	};

	class OrderBookEntry : public Serializable {
	protected:
		int m_instrumentId;
		bool m_isSnapshot;

		std::vector<OrderBookEntryItem> m_bids;
		std::vector<OrderBookEntryItem> m_asks;

	public:
		void Deserialize( Serializer & s ) override;

		int InstrumentId() const
		{
			return m_instrumentId;
		}

		const std::vector<OrderBookEntryItem> & Bids() const
		{
			return m_bids;
		}

		const std::vector<OrderBookEntryItem> & Asks() const
		{
			return m_asks;
		}
	};

	class OrderEntry : public Serializable {
	protected:
		int m_instrumentId;
		OrderType m_type;
		OrderLifetime m_lifetime;
		OrderDirection m_direction;
		OrderStatus m_status;
		int m_quantityInitial;
		int m_quantityRemaining;
		Number m_price;
		t_order_id m_id;

	public:
		void Deserialize( Serializer & s ) override;

		int InstrumentId() const
		{
			return m_instrumentId;
		}
		
		t_order_id Id() const
		{
			return m_id;
		}

		OrderType Type() const
		{
			return m_type;
		}

		OrderLifetime Lifetime() const
		{
			return m_lifetime;
		}

		OrderDirection Direction() const
		{
			return m_direction;
		}

		OrderStatus Status() const
		{
			return m_status;
		}

		int QuantityInitial() const
		{
			return m_quantityInitial;
		}

		int QuantityRemaining() const
		{
			return m_quantityRemaining;
		}

		const Number & Price() const
		{
			return m_price;
		}
	};

	class Instrument : public Serializable {
	protected:
		std::string m_symbol;
		Number m_minPriceIncrement;

	public:
		void Deserialize( Serializer & s ) override;

		const std::string & Symbol() const
		{
			return m_symbol;
		}

		const Number & MinPriceIncrement() const
		{
			return m_minPriceIncrement;
		}
	};

	class Position : public Serializable {
	protected:
		int m_instrumentId;
		int m_size;

		Number m_unrealizedPnl;
		Number m_realizedPnl;
		Number m_margin;
		Number m_maxRemovableMargin;
		Number m_entryPrice;
		Number m_entryNotionalValue;
		Number m_currentNotionalValue;
		Number m_partialLiquidationPrice;
		Number m_fullLiquidationPrice;

	public:
		void Deserialize( Serializer & s ) override;

		int InstrumentId() const
		{
			return m_instrumentId;
		}

		int Size() const
		{
			return m_size;
		}

		const Number & UnrealizedPnl() const
		{
			return m_unrealizedPnl;
		}

		const Number & RealizedPnl() const
		{
			return m_realizedPnl;
		}

		const Number & Margin() const
		{
			return m_margin;
		}

		const Number & MaxRemovableMargin() const
		{
			return m_maxRemovableMargin;
		}
	};

} // namespace zubr


#endif
