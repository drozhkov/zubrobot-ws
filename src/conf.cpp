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
	m_positionStart = doc["positionStart"].GetInt();
	m_positionMax = doc["positionMax"].GetInt();
	m_shift = doc["shift"].GetDouble();
	m_interest = doc["interest"].GetDouble();
	m_useConfigStartPosition = doc["useConfigStartPosition"].GetBool();

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
