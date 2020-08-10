/// JsonSerializer.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_JSON_SERIALIZER__H
#define __ZUBR_JSON_SERIALIZER__H


#include "rapidjson/document.h"

#include "Serializer.hpp"


namespace zubr {

	class JsonSerializer : public Serializer {
	protected:
		std::shared_ptr<rapidjson::Document> m_document;
		rapidjson::Value & m_value;

	public:
		JsonSerializer()
			: m_document( new rapidjson::Document )
			, m_value( m_document->SetObject() )
		{
		}

		JsonSerializer( const std::shared_ptr<rapidjson::Document> & document,
			rapidjson::Value & value )
			: m_document( document )
			, m_value( value )
		{
		}

		std::shared_ptr<Serializer> AddObject(
			const std::string & memberName = "" ) override;

		std::shared_ptr<Serializer> GetObject(
			const std::string & memberName = "" ) override;

		Serializer & Serialize(
			int64_t v, const std::string & memberName = "" ) override;

		Serializer & Serialize( const std::string & v,
			const std::string & memberName = "" ) override;

		Serializer & Serialize(
			Serializable & v, const std::string & memberName = "" ) override;

		Serializer & Deserialize( int & out,
			const std::string & memberName = "",
			int defaultValue = -1 ) override;

		Serializer & Deserialize( int64_t & out,
			const std::string & memberName = "",
			int64_t defaultValue = INT64_C( -1 ) ) override;

		Serializer & Deserialize( std::string & out,
			const std::string & memberName = "",
			const std::string & defaultValue = "" ) override;

		Serializer & Deserialize(
			Serializable & v, const std::string & memberName = "" ) override;

		Serializer & Deserialize( const std::function<void( Serializer &,
									  const std::string & memberName )> & add,
			const std::string & memberName = "" ) override;

		void ToString( std::string & out ) override;
		void FromString( const std::string & s ) override;
	};

	class JsonSerializerFactory : public SerializerFactory {
	public:
		std::shared_ptr<Serializer> Create() const override
		{
			return std::make_shared<JsonSerializer>();
		}
	};

} // namespace zubr


#endif
