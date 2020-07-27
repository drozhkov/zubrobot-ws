/// zuborbot-ws.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <iostream>

#include "zubr-connector-ws/ConnectorWs.hpp"


using namespace std;


static bool zubrIsAuthenticated = false;


void printUsage( const char * app )
{
	std::cout << app << ": <api-key> <api-secret>" << std::endl;
}

void messageHandler( zubr::ResponseWs & res )
{
	if ( res.Type() == zubr::ResponseType::Auth && res.IsOk() ) {
		zubrIsAuthenticated = true;
	}
	else {
		std::cout << static_cast<int>( res.Type() ) << ", " << res.IsOk()
				  << std::endl;
	}
}

int main( int argc, char * argv[] )
{
	if ( argc < 3 ) {
		printUsage( argv[0] );
		return -1;
	}

	try {
		zubr::ConnectorWs connector( argv[1], argv[2] );
		connector.SetMessageHandler( messageHandler );
		connector.Start();

		while ( true ) {
			if ( zubrIsAuthenticated ) {
				zubr::Number price( 6000, 0 );
				int instrumentId = 1;
				int quantity = 10;

				connector.Send<zubr::PlaceOrderRequestWs>( instrumentId,
					price,
					zubr::OrderDirection::Buy,
					quantity,
					zubr::OrderType::Limit,
					zubr::OrderLifetime::Gtc );
			}

			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		}
	}
	catch ( const std::exception & x ) {
		std::cerr << x.what() << std::endl;
	}

	return 0;
}
