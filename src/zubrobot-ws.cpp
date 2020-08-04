/// zuborbot-ws.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>

#include "zubr-connector-ws/ConnectorWs.hpp"

#include "conf.hpp"


using namespace std;


static volatile bool zubrIsAuthenticated = false;
static volatile bool zubrIsOrderPlaced = false;
zubr::conf zubrConf;
std::shared_ptr<zubr::ConnectorWs> zubrConnector;
zubr::Number zubrBestBuyPrice;
zubr::Number zubrBestSellPrice;


void printUsage( const char * app )
{
	std::cout << app << ": <conf-file-path>" << std::endl;
}

void connectHandler( zubr::AuthResponseWs & res )
{
	zubrIsAuthenticated = res.IsOk();

	if ( zubrIsAuthenticated ) {
		zubrConnector->Send<zubr::SubscribeRequestWs>(
			zubr::Channel::Instruments );

		zubrConnector->Send<zubr::SubscribeRequestWs>( zubr::Channel::Orders );

		zubrConnector->Send<zubr::SubscribeRequestWs>(
			zubr::Channel::OrderBook );

		zubrConnector->Send<zubr::SubscribeRequestWs>(
			zubr::Channel::Positions );
	}
}

void messageHandler( zubr::ResponseWs & res )
{
	std::cout << "type, ok: " << static_cast<int>( res.Type() ) << ", "
			  << res.IsOk() << std::endl;

	switch ( res.Type() ) {
		case zubr::ResponseType::ChannelOrderBook: {
			auto & r = static_cast<zubr::ChannelOrderBookResponseWs &>( res );
			std::cout << "order book"
					  << "\n";

			std::cout << "\t";
			std::cout << "instruments"
					  << "\n";

			for ( size_t i = 0; i < r.Entries().size(); ++i ) {
				if ( r.Entries()[i].InstrumentId()
					 == zubrConf.InstrumentId() ) {

					std::cout << "\t\t";
					std::cout << r.Entries()[i].InstrumentId() << "\n";
					std::cout << "\t\t\t";
					std::cout << "bids"
							  << "\n";

					for ( size_t j = 0; j < r.Entries()[i].Bids().size();
						  ++j ) {

						std::cout << "\t\t\t\t";
						std::cout << "price: "
								  << "("
								  << r.Entries()[i].Bids()[j].Price().Value()
								  << ") "
								  << "\n";

						if ( !zubrBestBuyPrice.HasValue()
							 || r.Entries()[i].Bids()[j].Price().Value()
									> zubrBestBuyPrice.Value() ) {

							zubrBestBuyPrice = r.Entries()[i].Bids()[j].Price();
						}
					}

					std::cout << "\t\t\t";
					std::cout << "asks"
							  << "\n";

					for ( size_t j = 0; j < r.Entries()[i].Asks().size();
						  ++j ) {

						std::cout << "\t\t\t\t";
						std::cout << "price: "
								  << "("
								  << r.Entries()[i].Asks()[j].Price().Value()
								  << ") "
								  << "\n";

						if ( !zubrBestSellPrice.HasValue()
							 || r.Entries()[i].Asks()[j].Price().Value()
									< zubrBestBuyPrice.Value() ) {

							zubrBestSellPrice
								= r.Entries()[i].Asks()[j].Price();
						}
					}

					std::cout << "\t\t\t";
					std::cout << "best buy: " << zubrBestBuyPrice.Value()
							  << "\n";

					std::cout << "\t\t\t";
					std::cout << "best sell: " << zubrBestSellPrice.Value()
							  << "\n";

					if ( !zubrIsOrderPlaced ) {
						zubr::Number buyPrice = zubrBestBuyPrice;
						buyPrice.Add( zubrBestSellPrice )
							.Div( 2 )
							.Sub( zubrConf.Interest() )
							.Sub( zubrConf.Shift() * zubrConf.PositionStart() );

						zubr::Number sellPrice = zubrBestBuyPrice;
						sellPrice.Add( zubrBestSellPrice )
							.Div( 2 )
							.Add( zubrConf.Interest() )
							.Sub( zubrConf.Shift() * zubrConf.PositionStart() );

						std::cout << "\t\t\t";
						std::cout << "buy price: " << buyPrice.Value() << "\n";

						std::cout << "\t\t\t";
						std::cout << "sell price: " << sellPrice.Value()
								  << "\n";

						zubrConnector->Send<zubr::PlaceOrderRequestWs>(
							zubrConf.InstrumentId(),
							buyPrice,
							zubr::OrderDirection::Buy,
							zubrConf.Quantity(),
							zubr::OrderType::Limit,
							zubr::OrderLifetime::Gtc );

						zubrConnector->Send<zubr::PlaceOrderRequestWs>(
							zubrConf.InstrumentId(),
							sellPrice,
							zubr::OrderDirection::Sell,
							zubrConf.Quantity(),
							zubr::OrderType::Limit,
							zubr::OrderLifetime::Gtc );

						zubrIsOrderPlaced = true;
					}
				}
			}
		}

		break;

		case zubr::ResponseType::ChannelPositions: {
			auto & r = static_cast<zubr::ChannelPositionsResponseWs &>( res );

			if ( r.InstrumentId() == zubrConf.InstrumentId() ) {
				std::cout << "positions"
						  << "\n";

				std::cout << "\t\t";
				std::cout << "margin: " << r.Margin().Value() << "\n";
			}
		}

		break;

		case zubr::ResponseType::ChannelOrders: {
			auto & r = static_cast<zubr::ChannelOrdersResponseWs &>( res );

			std::cout << "orders"
					  << "\n";

			for ( size_t i = 0; i < r.Entries().size(); ++i ) {
				if ( r.Entries()[i].InstrumentId()
					 == zubrConf.InstrumentId() ) {

					std::cout << "\t\t";
					std::cout << "status: "
							  << static_cast<int>( r.Entries()[i].Status() )
							  << "\n";

					std::cout << "\t\t";
					std::cout << "price: " << r.Entries()[i].Price().Value()
							  << "\n";

					std::cout << "\t\t";
					std::cout
						<< "remaining: " << r.Entries()[i].QuantityRemaining()
						<< "\n";
				}
			}
		}

		break;
	}
}

int main( int argc, char * argv[] )
{
	if ( argc < 2 ) {
		printUsage( argv[0] );
		return -1;
	}

	try {
		zubrConf.LoadFile( argv[1] );

		zubrConnector
			= std::make_shared<zubr::ConnectorWs>( zubrConf.Api().KeyId(),
				zubrConf.Api().KeySecret(),
				zubrConf.Api().Url(),
				zubrConf.Api().Host() );

		zubrConnector->SetConnectHandler( connectHandler );
		zubrConnector->SetMessageHandler( messageHandler );
		zubrConnector->Start();
		zubrConnector->Wait();
	}
	catch ( const std::exception & x ) {
		std::cerr << x.what() << std::endl;
	}

	return 0;
}
