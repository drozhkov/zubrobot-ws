/// conf.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBROBOT_CONF__H
#define __ZUBROBOT_CONF__H


#include <string>

#include "rapidjson/document.h"

#include "zubr-core/Logger.hpp"


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
		int m_positionSizeStart;
		int m_positionSizeMax;
		double m_shift;
		double m_interest;
		bool m_useConfigStartPositionSize;
		zubr::LogLevel m_logLevel;

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

		int PositionSizeStart() const
		{
			return m_positionSizeStart;
		}

		int PositionSizeMax() const
		{
			return m_positionSizeMax;
		}

		double Shift() const
		{
			return m_shift;
		}

		double Interest() const
		{
			return m_interest;
		}

		bool UseConfigStartPositionSize() const
		{
			return m_useConfigStartPositionSize;
		}

		zubr::LogLevel LogLevel() const
		{
			return m_logLevel;
		}

		void LoadJson( const std::string & json );
		void LoadFile( const std::string & filename );
	};

} // namespace zubr


#endif
