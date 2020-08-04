/// Response.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <iostream>

#include "rapidjson/document.h"

#include "../include/zubr-connector-ws/Response.hpp"


//#define ZUBR_DEBUG


using namespace zubr;


struct zubr::DeserializerWs {
	rapidjson::Document Document;
	rapidjson::Value * Value;


	void MoveToMember( const std::string & name )
	{
		Value = &( ( *Value )[name.c_str()] );
	}
};


int64_t Deserializable::GetIntOrDefault(
	DeserializerWs * o, const std::string & memberName, int64_t defaultValue )
{

	return ( o->Value->HasMember( memberName.c_str() )
				 ? ( *( o->Value ) )[memberName.c_str()].GetInt64()
				 : defaultValue );
}

std::string Deserializable::GetStringOrDefault( DeserializerWs * o,
	const std::string & memberName,
	const std::string & defaultValue )
{

	return ( o->Value->HasMember( memberName.c_str() )
				 ? ( *( o->Value ) )[memberName.c_str()].GetString()
				 : defaultValue );
}


std::shared_ptr<ResponseWs> ResponseWs::Deserialize( const std::string & in,
	const std::function<ResponseType( id_t id )> & typeResolver )
{
	if ( in.empty() ) {
		return std::make_shared<ResponseWs>( ResponseType::_undef );
	}

	DeserializerWs deserializer;
	deserializer.Document.Parse( in.c_str() );
	deserializer.Value = &deserializer.Document;

	auto id = GetIntOrDefault( &deserializer, "id" );

	deserializer.Value = &( deserializer.Document["result"] );
	auto channelName = GetStringOrDefault( &deserializer, "channel" );

#ifdef ZUBR_DEBUG
	std::cout << "id, channelName: " << id << ", " << channelName << std::endl;
#endif

	std::shared_ptr<ResponseWs> result;

	if ( !channelName.empty() ) {
		auto channel = ChannelEnumHelper::FromChannelName( channelName );

		switch ( channel ) {
			case Channel::OrderBook:
				result.reset( new ChannelOrderBookResponseWs );

				break;

			case Channel::Orders:
				result.reset( new ChannelOrdersResponseWs );

				break;

			case Channel::Positions:
				result.reset( new ChannelPositionsResponseWs );

				break;
		}
	}

	if ( !result ) {
		auto type = typeResolver( id );

#ifdef ZUBR_DEBUG
		std::cout << "type: " << static_cast<int>( type ) << std::endl;
#endif

		switch ( type ) {
			case ResponseType::Auth:
				result.reset( new AuthResponseWs );
				break;

			case ResponseType::PlaceOrder:
				break;
		}
	}

	if ( !result ) {
		result.reset( new ResponseWs( ResponseType::_undef ) );
	}

	result->m_id = id;

	result->Deserialize( result, &deserializer );

	return result;
}

void ResponseWs::Deserialize(
	const std::shared_ptr<ResponseWs> & out, DeserializerWs * o )
{
	if ( o->Value->HasMember( "data" ) ) {
		o->MoveToMember( "data" );
	}

	auto stringValue = GetStringOrDefault( o, "tag" );

	if ( "ok" == stringValue ) {
		m_isOk = true;
	}
	else if ( "err" == stringValue ) {
		m_isOk = false;
	}

	if ( o->Value->HasMember( "value" ) ) {
		o->MoveToMember( "value" );
		Deserialize( o );
	}
}


void Number::Deserialize( DeserializerWs * o )
{
	if ( o->Value->IsNull() ) {
		return;
	}

	m_significand = GetIntOrDefault( o, "mantissa" );
	Exponent( GetIntOrDefault( o, "exponent" ) );
}


void Time::Deserialize( DeserializerWs * o )
{
}


void OrderBookEntryItem::Deserialize( DeserializerWs * o )
{
	m_quantity = GetIntOrDefault( o, "size" );

	o->MoveToMember( "price" );
	m_price.Deserialize( o );
}


void OrderBookEntry::Deserialize( DeserializerWs * o )
{
	m_instrumentId = GetIntOrDefault( o, "instrumentId" );

	auto & v = *o->Value;
	const auto & bids = v["bids"].GetArray();

	for ( rapidjson::SizeType i = 0; i < bids.Size(); ++i ) {
		OrderBookEntryItem item;

		o->Value = &( bids[i] );
		item.Deserialize( o );

		m_bids.push_back( item );
	}

	const auto & asks = v["asks"].GetArray();

	for ( rapidjson::SizeType i = 0; i < asks.Size(); ++i ) {
		OrderBookEntryItem item;

		o->Value = &( asks[i] );
		item.Deserialize( o );

		m_asks.push_back( item );
	}
}


void OrderEntry::Deserialize( DeserializerWs * o )
{
	m_instrumentId = GetIntOrDefault( o, "instrumentId" );
	m_type
		= OrderEnumHelper::FromOrderTypeName( GetStringOrDefault( o, "type" ) );

	m_lifetime = OrderEnumHelper::FromOrderLifetimeName(
		GetStringOrDefault( o, "timeInForce" ) );

	m_direction = OrderEnumHelper::FromOrderDirectionName(
		GetStringOrDefault( o, "side" ) );

	m_status = OrderEnumHelper::FromOrderStatusName(
		GetStringOrDefault( o, "status" ) );

	o->MoveToMember( "price" );
	m_price.Deserialize( o );
}


void AuthResponseWs::Deserialize( DeserializerWs * o )
{
	m_userId = GetIntOrDefault( o, "userId" );
}


void ChannelOrdersResponseWs::Deserialize( DeserializerWs * o )
{
	auto type = GetStringOrDefault( o, "type" );

	if ( type == "update" ) {
		o->MoveToMember( "payload" );

		OrderEntry entry;
		entry.Deserialize( o );

		m_entries.push_back( entry );
	}
}


void ChannelOrderBookResponseWs::Deserialize( DeserializerWs * o )
{
	auto & v = *o->Value;

	for ( auto & m : v.GetObject() ) {
		o->Value = &( v[m.name.GetString()] );

		OrderBookEntry entry;
		entry.Deserialize( o );

		m_entries.push_back( entry );
	}
}


void ChannelPositionsResponseWs::Deserialize( DeserializerWs * o )
{
	auto type = GetStringOrDefault( o, "type" );

	if ( type == "update" ) {
		o->MoveToMember( "payload" );

		m_instrumentId = GetIntOrDefault( o, "instrumentId" );
		m_quantity = GetIntOrDefault( o, "size" );

		auto & v = *o->Value;
		o->Value = &v["unrealizedPnl"];
		m_unrealizedPnl.Deserialize( o );

		o->Value = &v["realizedPnl"];
		m_realizedPnl.Deserialize( o );

		o->Value = &v["margin"];
		m_margin.Deserialize( o );

		o->Value = &v["maxRemovableMargin"];
		m_maxRemovableMargin.Deserialize( o );

		o->Value = &v["entryPrice"];
		m_entryPrice.Deserialize( o );

		o->Value = &v["entryNotionalValue"];
		m_entryNotionalValue.Deserialize( o );

		o->Value = &v["currentNotionalValue"];
		m_currentNotionalValue.Deserialize( o );

		o->Value = &v["partialLiquidationPrice"];
		m_partialLiquidationPrice.Deserialize( o );

		o->Value = &v["fullLiquidationPrice"];
		m_fullLiquidationPrice.Deserialize( o );
	}
}
