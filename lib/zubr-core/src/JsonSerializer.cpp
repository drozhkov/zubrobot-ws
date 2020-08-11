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

/// JsonSerializer.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "../include/zubr-core/JsonSerializer.hpp"


using namespace zubr;


std::shared_ptr<Serializer> JsonSerializer::AddObject(
	const std::string & memberName )
{

	rapidjson::Value object( rapidjson::kObjectType );
	m_value.AddMember( rapidjson::Value().SetString(
						   memberName.c_str(), m_document->GetAllocator() ),
		object,
		m_document->GetAllocator() );

	return std::shared_ptr<Serializer>(
		new JsonSerializer( m_document, m_value[memberName.c_str()] ) );
}

std::shared_ptr<Serializer> JsonSerializer::GetObject(
	const std::string & memberName )
{
	if ( !m_value.HasMember( memberName.c_str() )
		 || m_value[memberName.c_str()].IsNull() ) {

		return nullptr;
	}

	return std::shared_ptr<Serializer>(
		new JsonSerializer( m_document, m_value[memberName.c_str()] ) );
}

Serializer & JsonSerializer::Serialize(
	int64_t v, const std::string & memberName )
{

	m_value.AddMember( rapidjson::Value().SetString(
						   memberName.c_str(), m_document->GetAllocator() ),
		v,
		m_document->GetAllocator() );

	return *this;
}

Serializer & JsonSerializer::Serialize(
	const std::string & v, const std::string & memberName )
{

	m_value.AddMember( rapidjson::Value().SetString(
						   memberName.c_str(), m_document->GetAllocator() ),
		rapidjson::Value().SetString( v.c_str(), m_document->GetAllocator() ),
		m_document->GetAllocator() );

	return *this;
}

Serializer & JsonSerializer::Serialize(
	Serializable & v, const std::string & memberName )
{

	auto serializer = AddObject( memberName );
	v.Serialize( *serializer );

	return *this;
}

Serializer & JsonSerializer::Deserialize(
	int & out, const std::string & memberName, int defaultValue )
{
	int64_t t;
	Deserialize( t, memberName, defaultValue );
	out = t;

	return *this;
}

Serializer & JsonSerializer::Deserialize(
	int64_t & out, const std::string & memberName, int64_t defaultValue )

{

	if ( memberName.empty() ) {
		out = m_value.GetInt64();
	}
	else {
		out = ( m_value.HasMember( memberName.c_str() )
					? m_value[memberName.c_str()].GetInt64()
					: defaultValue );
	}

	return *this;
}

Serializer & JsonSerializer::Deserialize( std::string & out,
	const std::string & memberName,
	const std::string & defaultValue )
{

	if ( memberName.empty() ) {
		out.assign( m_value.GetString() );
	}
	else {
		out.assign( m_value.HasMember( memberName.c_str() )
						? m_value[memberName.c_str()].GetString()
						: defaultValue );
	}

	return *this;
}

Serializer & JsonSerializer::Deserialize(
	Serializable & v, const std::string & memberName )
{

	auto o = GetObject( memberName );

	if ( o ) {
		v.Deserialize( *o );
	}

	return *this;
}

Serializer & JsonSerializer::Deserialize(
	const std::function<void( Serializer &, const std::string & memberName )> &
		add,
	const std::string & memberName )
{

	if ( !memberName.empty() ) {
		auto array = m_value[memberName.c_str()].GetArray();

		for ( rapidjson::SizeType i = 0; i < array.Size(); ++i ) {
			JsonSerializer serializer( m_document, array[i] );
			add( serializer, "" );
		}
	}
	else {
		for ( auto & member : m_value.GetObject() ) {
			JsonSerializer serializer(
				m_document, m_value[member.name.GetString()] );

			add( serializer, member.name.GetString() );
		}
	}

	return *this;
}

void JsonSerializer::ToString( std::string & out )
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer( buffer );
	m_document->Accept( writer );
	out.assign( buffer.GetString() );
}

void JsonSerializer::FromString( const std::string & s )
{
	m_document->Parse( s.c_str() );
	m_value = m_document->GetObject();
}
