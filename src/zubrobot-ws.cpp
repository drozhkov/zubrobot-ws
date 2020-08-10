/// zubrobot-ws.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <iostream>

#include "zubr-core/Logger.hpp"

#include "bot.hpp"
#include "conf.hpp"


void printUsage( const char * app )
{
	std::cout << app << ": <conf-file-path>" << std::endl;
}

int main( int argc, char * argv[] )
{
	if ( argc < 2 ) {
		printUsage( argv[0] );
		return -1;
	}

	try {
		zubr::conf conf;
		conf.LoadFile( argv[1] );

		ZUBR_LOG_SET_LEVEL( conf.LogLevel() );

		zubr::bot bot( conf );
		bot.start();
		bot.wait();
	}
	catch ( const std::exception & x ) {
		std::cerr << x.what() << std::endl;
		return -1;
	}

	return 0;
}
