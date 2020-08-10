/// Response.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "rapidjson/document.h"

#include "../include/zubr-connector-ws/Response.hpp"


using namespace zubr;


std::shared_ptr<ResponseWs> ResponseWs::Deserialize( Serializer & s,
	const std::string & in,
	const std::function<ResponseType( id_t id )> & typeResolver )
{

	if ( in.empty() ) {
		return std::make_shared<ResponseWs>( ResponseType::_undef );
	}

	s.FromString( in );

	id_t id;
	s.Deserialize( id, "id" );
	auto resResult = s.GetObject( "result" );

	std::string channelName;
	resResult->Deserialize( channelName, "channel" );

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

			case Channel::Instruments:
				result.reset( new ChannelInstrumentsResponseWs );

				break;
		}
	}

	if ( !result ) {
		auto type = typeResolver( id );

		switch ( type ) {
			case ResponseType::Auth:
				result.reset( new AuthResponseWs );
				break;

			case ResponseType::PlaceOrder:
				result.reset( new PlaceOrderResponseWs );
				break;
		}
	}

	if ( !result ) {
		result.reset( new ResponseWs( ResponseType::_undef ) );
	}

	result->m_id = id;
	result->Deserialize( result, *resResult );

	return result;
}

void ResponseWs::Deserialize(
	const std::shared_ptr<ResponseWs> & out, Serializer & s )
{

	auto data = s.GetObject( "data" );
	Serializer & s_ = data ? *data : s;

	std::string stringValue;
	s_.Deserialize( stringValue, "tag" );

	if ( "ok" == stringValue ) {
		m_isOk = true;
	}
	else if ( "err" == stringValue ) {
		m_isOk = false;
	}

	auto value = s_.GetObject( "value" );

	if ( value ) {
		if ( m_isOk ) {
			Deserialize( *value );
		}
		else {
			value->Deserialize( m_errorCodeName, "code" );
		}
	}
}


void AuthResponseWs::Deserialize( Serializer & s )
{
	s.Deserialize( m_userId, "userId" );
}


void PlaceOrderResponseWs::Deserialize( Serializer & s )
{
	std::string stringValue;
	s.Deserialize( stringValue );
	m_orderId = std::stoll( stringValue );
}


void ChannelOrdersResponseWs::Deserialize( Serializer & s )
{
	std::string stringValue;

	s.Deserialize( stringValue, "type" );

	if ( "update" == stringValue ) {
		OrderEntry entry;
		s.Deserialize( entry, "payload" );

		m_entries[entry.InstrumentId()] = entry;
	}
}


void ChannelOrderBookResponseWs::Deserialize( Serializer & s )
{
	s.Deserialize( m_entries );
}


void ChannelPositionsResponseWs::Deserialize( Serializer & s )
{
	std::string stringValue;

	s.Deserialize( stringValue, "type" );

	if ( "update" == stringValue ) {
		Position p;
		s.Deserialize( p, "payload" );

		m_entries[p.InstrumentId()] = p;
	}
	else if ( "snapshot" == stringValue ) {
		auto s_ = s.GetObject( "payload" );
		s_->Deserialize( m_entries );
	}
}


void ChannelInstrumentsResponseWs::Deserialize( Serializer & s )
{
	s.Deserialize( m_list );
}
