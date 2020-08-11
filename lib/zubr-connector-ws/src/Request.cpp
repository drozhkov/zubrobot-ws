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

/// Request.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <chrono>
#include <iostream>

#include "../include/zubr-connector-ws/Digest.hpp"

#include "../include/zubr-connector-ws/Request.hpp"


using namespace zubr;


const std::string zubr::AuthRequestWs::ReqMethodName(
	"loginSessionByApiToken" );

const std::string zubr::PlaceOrderRequestWs::ReqMethodName( "placeOrder" );
const std::string zubr::ReplaceOrderRequestWs::ReqMethodName( "replaceOrder" );
const std::string zubr::CancelOrderRequestWs::ReqMethodName( "cancelOrder" );


void RequestWs::Serialize( std::string & out, Serializer & s, RequestWs & req )
{
	req.SerializeBase( s );
	s.ToString( out );
}

void RequestWs::SerializeBase( Serializer & s )
{
	s.Serialize( m_methodId, "method" ).Serialize( m_id, "id" );

	auto params = s.AddObject( "params" );

	if ( m_methodId == MethodIdRequest ) {
		params->Serialize( *this, "data" );
	}
	else {
		params->Serialize(
			ChannelEnumHelper::ToString( m_channel ), "channel" );
	}
}


void AuthRequestWs::Serialize( Serializer & s )
{
	s.Serialize( m_methodName, "method" );
	auto params = s.AddObject( "params" );

	auto now = Time::Now();
	params->Serialize( now, "time" );
	params->Serialize( m_keyId, "apiKey" );

	std::string digest;
	Digest::Calculate( digest, m_keyId, m_keySecret, now.Seconds() );
	params->Serialize( digest, "hmacDigest" );
}


void PlaceOrderRequestWs::Serialize( Serializer & s )
{
	s.Serialize( m_methodName, "method" );
	s.AddObject( "params" )
		->Serialize( m_instrumentId, "instrument" )
		.Serialize( m_quantity, "size" )
		.Serialize( OrderEnumHelper::ToString( m_type ), "type" )
		.Serialize( OrderEnumHelper::ToString( m_direction ), "side" )
		.Serialize( OrderEnumHelper::ToString( m_lifetime ), "timeInForce" )
		.Serialize( m_price, "price" );
}

void ReplaceOrderRequestWs::Serialize( Serializer & s )
{
	s.Serialize( m_methodName, "method" );
	s.AddObject( "params" )
		->Serialize( m_orderId, "orderId" )
		.Serialize( m_price, "price" )
		.Serialize( m_quantity, "size" );
}

void CancelOrderRequestWs::Serialize( Serializer & s )
{
	s.Serialize( m_methodName, "method" );
	s.Serialize( m_orderId, "params" );
}
