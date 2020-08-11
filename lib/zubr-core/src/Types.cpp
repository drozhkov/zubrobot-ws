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

	o.Deserialize( m_id, "id" );
	o.Deserialize( m_instrumentId, "instrument" );

	o.Deserialize( stringValue, "type" );
	m_type = OrderEnumHelper::FromOrderTypeName( stringValue );

	o.Deserialize( stringValue, "timeInForce" );
	m_lifetime = OrderEnumHelper::FromOrderLifetimeName( stringValue );

	o.Deserialize( stringValue, "side" );
	m_direction = OrderEnumHelper::FromOrderDirectionName( stringValue );

	o.Deserialize( stringValue, "status" );
	m_status = OrderEnumHelper::FromOrderStatusName( stringValue );

	o.Deserialize( m_price, "price" );
	o.Deserialize( m_quantityInitial, "initialSize" );
	o.Deserialize( m_quantityRemaining, "remainingSize" );
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
