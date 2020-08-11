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

/// conf.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <fstream>
#include <sstream>

#include "zubr-core/Exception.hpp"

#include "conf.hpp"


using namespace zubr;


void confApi::Deserialize( rapidjson::Value & v )
{
	m_url = v["url"].GetString();
	m_host = v["host"].GetString();
	m_keyId = v["keyId"].GetString();
	m_keySecret = v["keySecret"].GetString();
}


void conf::LoadJson( const std::string & json )
{
	if ( json.empty() ) {
		throw zubr::Exception();
	}

	rapidjson::Document doc;
	doc.Parse( json.c_str() );

	m_instrumentId = doc["instrumentId"].GetInt();
	m_quantity = doc["quantity"].GetInt();
	m_positionSizeStart = doc["positionSizeStart"].GetInt();
	m_positionSizeMax = doc["positionSizeMax"].GetInt();
	m_shift = doc["shift"].GetDouble();
	m_interest = doc["interest"].GetDouble();
	m_useConfigStartPositionSize = doc["useConfigStartPositionSize"].GetBool();

	std::string stringValue = doc["logLevel"].GetString();

	if ( stringValue == "debug" ) {
		m_logLevel = LogLevel::Debug;
	}
	else if ( stringValue == "info" ) {
		m_logLevel = LogLevel::Info;
	}
	else {
		m_logLevel = LogLevel::Error;
	}

	m_api.Deserialize( doc["api"] );
}

void conf::LoadFile( const std::string & filename )
{
	std::ifstream fs( filename );

	if ( !fs ) {
		throw zubr::Exception();
	}

	std::stringstream ss;
	ss << fs.rdbuf();
	fs.close();

	LoadJson( ss.str() );
}
