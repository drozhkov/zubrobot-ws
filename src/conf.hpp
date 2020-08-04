/// conf.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBROBOT_CONF__H
#define __ZUBROBOT_CONF__H


#include <string>

#include "rapidjson/document.h"


namespace zubr {

	class confApi {
	protected:
		std::string m_url;
		std::string m_host;
		std::string m_keyId;
		std::string m_keySecret;

	public:
		void Deserialize( rapidjson::Value & v );

		const std::string & Url() const
		{
			return m_url;
		}

		const std::string & Host() const
		{
			return m_host;
		}

		const std::string & KeyId() const
		{
			return m_keyId;
		}

		const std::string & KeySecret() const
		{
			return m_keySecret;
		}
	};

	class conf {
	protected:
		confApi m_api;

		int m_instrumentId;
		int m_quantity;
		int m_positionStart;
		int m_positionMax;
		double m_shift;
		double m_interest;
		bool m_useConfigStartPosition;

	public:
		const confApi & Api() const
		{
			return m_api;
		}

		int InstrumentId() const
		{
			return m_instrumentId;
		}

		int Quantity() const
		{
			return m_quantity;
		}

		int PositionStart() const
		{
			return m_positionStart;
		}

		int PositionMax() const
		{
			return m_positionMax;
		}

		double Shift() const
		{
			return m_shift;
		}

		double Interest() const
		{
			return m_interest;
		}

		bool UseConfigStartPosition() const
		{
			return m_useConfigStartPosition;
		}

		void LoadJson( const std::string & json );
		void LoadFile( const std::string & filename );
	};

} // namespace zubr


#endif
