/// Request.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <chrono>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "../include/zubr-connector-ws/Digest.hpp"

#include "../include/zubr-connector-ws/Request.hpp"


using namespace zubr;


struct zubr::SerializerWs {
	rapidjson::Document Document;
	rapidjson::Value * Value;
};


const std::string zubr::AuthRequestWs::ReqMethodName(
	"loginSessionByApiToken" );

const std::string zubr::PlaceOrderRequestWs::ReqMethodName( "placeOrder" );


std::shared_ptr<SerializerWs> RequestWs::CreateSerializer()
{
	auto result = std::make_shared<SerializerWs>();
	result->Value = &result->Document.SetObject();

	return result;
}

void RequestWs::Serialize( std::string & out, RequestWs & req )
{
	auto serializer = CreateSerializer();
	req.Serialize( serializer.get(), req );

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer( buffer );
	serializer->Document.Accept( writer );

	out.assign( buffer.GetString() );
}

void RequestWs::Serialize( SerializerWs * o, RequestWs & req )
{
	rapidjson::Value v( rapidjson::kObjectType );
	auto node = o->Value;
	o->Value = &v;

	node->AddMember( "method", m_methodId, o->Document.GetAllocator() );
	node->AddMember( "id", m_id, o->Document.GetAllocator() );

	rapidjson::Value params( rapidjson::kObjectType );

	if ( m_methodId == MethodIdRequest ) {
		req.Serialize( o );
		params.AddMember( "data", *o->Value, o->Document.GetAllocator() );
	}
	else {
		params.AddMember( "channel",
			rapidjson::Value().Set( ChannelEnumHelper::ToString( m_channel ) ),
			o->Document.GetAllocator() );
	}

	node->AddMember( "params", params, o->Document.GetAllocator() );
}


void Number::Serialize( SerializerWs * o )
{
	o->Value->AddMember(
		"mantissa", m_significand, o->Document.GetAllocator() );

	o->Value->AddMember( "exponent", m_exponent, o->Document.GetAllocator() );
}


void Time::Serialize( SerializerWs * o )
{
	o->Value->AddMember( "seconds", m_seconds, o->Document.GetAllocator() );
	o->Value->AddMember( "nanos", m_nanoseconds, o->Document.GetAllocator() );
}


void AuthRequestWs::Serialize( SerializerWs * o )
{
	auto v = o->Value;

	o->Value->AddMember( "method",
		rapidjson::Value().Set( m_methodName.c_str() ),
		o->Document.GetAllocator() );

	auto now = Time::Now();
	rapidjson::Value time( rapidjson::kObjectType );
	o->Value = &time;
	now.Serialize( o );
	o->Value = v;

	rapidjson::Value params( rapidjson::kObjectType );
	params.AddMember( "time", time, o->Document.GetAllocator() );
	params.AddMember( "apiKey",
		rapidjson::Value().Set( m_keyId.c_str() ),
		o->Document.GetAllocator() );

	std::string digest;
	Digest::Calculate( digest, m_keyId, m_keySecret, now.Seconds() );
	params.AddMember( "hmacDigest",
		rapidjson::Value().SetString(
			digest.c_str(), o->Document.GetAllocator() ),
		o->Document.GetAllocator() );

	o->Value->AddMember( "params", params, o->Document.GetAllocator() );
}


void PlaceOrderRequestWs::Serialize( SerializerWs * o )
{
	o->Value->AddMember( "method",
		rapidjson::Value().Set( m_methodName.c_str() ),
		o->Document.GetAllocator() );

	rapidjson::Value params( rapidjson::kObjectType );
	params.AddMember(
		"instrument", m_instrumentId, o->Document.GetAllocator() );

	params.AddMember( "size", m_quantity, o->Document.GetAllocator() );
	params.AddMember( "type",
		rapidjson::Value().Set( OrderEnumHelper::ToString( m_type ) ),
		o->Document.GetAllocator() );

	params.AddMember( "side",
		rapidjson::Value().Set( OrderEnumHelper::ToString( m_direction ) ),
		o->Document.GetAllocator() );

	params.AddMember( "timeInForce",
		rapidjson::Value().Set( OrderEnumHelper::ToString( m_lifetime ) ),
		o->Document.GetAllocator() );

	rapidjson::Value price( rapidjson::kObjectType );
	auto v = o->Value;
	o->Value = &price;
	m_price.Serialize( o );
	params.AddMember( "price", price, o->Document.GetAllocator() );
	o->Value = v;

	o->Value->AddMember( "params", params, o->Document.GetAllocator() );
}
