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
