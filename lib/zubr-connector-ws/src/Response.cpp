/// Response.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "rapidjson/document.h"

#include "../include/zubr-connector-ws/Response.hpp"


using namespace zubr;


struct zubr::DeserializerWs {
	rapidjson::Document Document;
	rapidjson::Value * Value;
};


int ResponseWs::GetIntOrDefault(
	DeserializerWs * o, const std::string & memberName, int defaultValue )
{

	return ( o->Value->HasMember( memberName.c_str() )
				 ? ( *( o->Value ) )[memberName.c_str()].GetInt()
				 : defaultValue );
}

std::string ResponseWs::GetStringOrDefault( DeserializerWs * o,
	const std::string & memberName,
	const std::string & defaultValue )
{

	return ( o->Value->HasMember( memberName.c_str() )
				 ? ( *( o->Value ) )[memberName.c_str()].GetString()
				 : defaultValue );
}

void ResponseWs::Deserialize( ResponseWs & out, const std::string & in )
{
	if ( in.empty() ) {
		return;
	}

	DeserializerWs deserializer;
	deserializer.Document.Parse( in.c_str() );
	deserializer.Value = &deserializer.Document;
	out.Deserialize( out, &deserializer );
}

void ResponseWs::Deserialize( ResponseWs & out, DeserializerWs * o )
{
	m_id = GetIntOrDefault( o, "id" );
	o->Value = &( *( o->Value ) )["result"];

	auto stringValue = GetStringOrDefault( o, "channel" );

	if ( !stringValue.empty() ) {
		m_channelName = stringValue;
	}

	if ( o->Value->HasMember( "data" ) ) {
		o->Value = &( *( o->Value ) )["data"];
	}

	stringValue = GetStringOrDefault( o, "tag" );

	if ( "ok" == stringValue ) {
		m_isOk = true;
	}
	else if ( "err" == stringValue ) {
		m_isOk = false;
	}

	if ( o->Value->HasMember( "value" ) ) {
		o->Value = &( *( o->Value ) )["value"];
		Deserialize( o );
	}
}


void Number::Deserialize( DeserializerWs * o )
{
}


void AuthResponseWs::Deserialize( DeserializerWs * o )
{
	m_userId = GetIntOrDefault( o, "userId" );
}
