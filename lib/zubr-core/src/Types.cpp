/// Types.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "../include/zubr-core/Types.hpp"


using namespace zubr;


void Number::Serialize( Serializer & o )
{
	o.Serialize( m_significand, "mantissa" )
		.Serialize( m_exponent, "exponent" );
}

void Number::Deserialize( Serializer & o )
{
	o.Deserialize( m_significand, "mantissa", m_significand )
		.Deserialize( m_exponent, "exponent" );

	Exponent( m_exponent );
}


void Time::Deserialize( Serializer & o )
{
}

void Time::Serialize( Serializer & o )
{
	o.Serialize( m_seconds, "seconds" ).Serialize( m_nanoseconds, "nanos" );
}


void OrderBookEntryItem::Deserialize( Serializer & o )
{
	o.Deserialize( m_quantity, "size" ).Deserialize( m_price, "price" );
}


void OrderBookEntry::Deserialize( Serializer & o )
{
	o.Deserialize( m_instrumentId, "instrumentId" );
	o.Deserialize( m_bids, "bids" );
	o.Deserialize( m_asks, "asks" );
}


void OrderEntry::Deserialize( Serializer & o )
{
	std::string stringValue;

	o.Deserialize( m_instrumentId, "instrumentId" );

	o.Deserialize( stringValue, "type" );
	m_type = OrderEnumHelper::FromOrderTypeName( stringValue );

	o.Deserialize( stringValue, "timeInForce" );
	m_lifetime = OrderEnumHelper::FromOrderLifetimeName( stringValue );

	o.Deserialize( stringValue, "side" );
	m_direction = OrderEnumHelper::FromOrderDirectionName( stringValue );

	o.Deserialize( stringValue, "status" );
	m_status = OrderEnumHelper::FromOrderStatusName( stringValue );

	o.Deserialize( m_price, "price" );
}


void Instrument::Deserialize( Serializer & s )
{
	s.Deserialize( m_symbol, "symbol" );
	s.Deserialize( m_minPriceIncrement, "minPriceIncrement" );
}


void Position::Deserialize( Serializer & s )
{
	s.Deserialize( m_instrumentId, "instrumentId" )
		.Deserialize( m_size, "size" )
		.Deserialize( m_unrealizedPnl, "unrealizedPnl" )
		.Deserialize( m_realizedPnl, "realizedPnl" )
		.Deserialize( m_margin, "margin" )
		.Deserialize( m_maxRemovableMargin, "maxRemovableMargin" )
		.Deserialize( m_entryPrice, "entryPrice" )
		.Deserialize( m_entryNotionalValue, "entryNotionalValue" )
		.Deserialize( m_currentNotionalValue, "currentNotionalValue" )
		.Deserialize( m_partialLiquidationPrice, "partialLiquidationPrice" )
		.Deserialize( m_fullLiquidationPrice, "fullLiquidationPrice" );
}
