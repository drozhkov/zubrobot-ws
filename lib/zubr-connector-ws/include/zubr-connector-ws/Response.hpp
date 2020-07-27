/// Response.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_RESPONSE__H
#define __ZUBR_CONNECTOR_WS_RESPONSE__H


#include <string>

#include "Types.hpp"


namespace zubr {

	enum class ResponseType { _undef = 0, Auth, PlaceOrder };

	class ResponseWs : public Deserializable {
	protected:
		int m_id;
		bool m_isOk;
		std::string m_channelName;
		ResponseType m_type;

	protected:
		static int GetIntOrDefault( DeserializerWs * o,
			const std::string & memberName,
			int defaultValue = -1 );

		static std::string GetStringOrDefault( DeserializerWs * o,
			const std::string & memberName,
			const std::string & defaultValue = "" );

	public:
		ResponseWs( ResponseType type )
			: m_type( type )
			, m_isOk( false )
		{
		}

		static void Deserialize( ResponseWs & out, const std::string & in );

		void Deserialize( DeserializerWs * o ) override
		{
		}

		void Deserialize( ResponseWs & out, DeserializerWs * o );

		ResponseType Type() const
		{
			return m_type;
		}

		int Id() const
		{
			return m_id;
		}

		bool IsOk() const
		{
			return m_isOk;
		}
	};

	/// @brief authentication response
	class AuthResponseWs : public ResponseWs {
	protected:
		int m_userId;

	public:
		AuthResponseWs()
			: ResponseWs( ResponseType::Auth )
		{
		}

		void Deserialize( DeserializerWs * o ) override;

		int UserId() const
		{
			return m_userId;
		}
	};

} // namespace zubr


#endif
