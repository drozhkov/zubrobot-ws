/// bot.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "zubr-core/Logger.hpp"

#include "bot.hpp"


using namespace zubr;


void bot::placeOrder( OrderDirection direction, bool & isPlaced )
{
	zubr::Number price = m_bestBuyPrice;
	price.Add( m_bestSellPrice ).Div( 2 );


	if ( OrderDirection::Buy == direction ) {
		price.Sub( m_conf.Interest() );
	}
	else {
		price.Add( m_conf.Interest() );
	}

	price.Sub( m_conf.Shift() * m_positionSize ).ModRing( m_minPriceIncrement );

	ZUBR_LOG_INFO( "placing " << OrderEnumHelper::ToString( direction )
							  << " order, price: " << price.Value() );

	auto req = std::make_shared<PlaceOrderRequestWs>( m_conf.InstrumentId(),
		price,
		direction,
		m_conf.Quantity(),
		zubr::OrderType::Limit,
		zubr::OrderLifetime::Gtc );

	auto reqId = m_connector.Send( *req );
	m_orderReqMap[reqId] = req;

	isPlaced = true;
}

void bot::connectHandler( zubr::AuthResponseWs & res )
{
	if ( res.IsOk() ) {
		m_connector.Send<zubr::SubscribeRequestWs>(
			zubr::Channel::Instruments );

		m_connector.Send<zubr::SubscribeRequestWs>( zubr::Channel::Orders );
		m_connector.Send<zubr::SubscribeRequestWs>( zubr::Channel::OrderBook );
		m_connector.Send<zubr::SubscribeRequestWs>( zubr::Channel::Positions );
	}
}

void bot::messageHandler( zubr::ResponseWs & res )
{
	ZUBR_LOG_DEBUG(
		"type: " << static_cast<int>( res.Type() ) << ", ok: " << res.IsOk() );

	switch ( res.Type() ) {
		case ResponseType::PlaceOrder: {
			auto & r = static_cast<zubr::PlaceOrderResponseWs &>( res );
			auto req = m_orderReqMap[r.Id()];

			if ( !r.IsOk() ) {
				if ( req->Direction() == OrderDirection::Buy ) {
					ZUBR_LOG_ERROR(
						"BUY order rejected: " << r.ErrorCodeName() );

					m_isBuyOrderPlaced = false;
				}
				else if ( req->Direction() == OrderDirection::Sell ) {
					ZUBR_LOG_ERROR(
						"SELL order rejected: " << r.ErrorCodeName() );

					m_isSellOrderPlaced = false;
				}
			}
			else {
				m_orderMap[r.OrderId()] = req;
			}

			m_orderReqMap.erase( r.Id() );
		} break;

		case zubr::ResponseType::ChannelInstruments: {
			auto & r = static_cast<zubr::ChannelInstrumentsResponseWs &>( res );
			auto it = r.List().find( m_conf.InstrumentId() );

			if ( it != r.List().end() ) {
				m_minPriceIncrement = it->second.MinPriceIncrement();
			}
		} break;

		case zubr::ResponseType::ChannelOrderBook: {
			auto & r = static_cast<zubr::ChannelOrderBookResponseWs &>( res );
			auto it = r.Entries().find( m_conf.InstrumentId() );

			if ( r.Entries().end() != it ) {
				for ( size_t i = 0; i < it->second.Bids().size(); ++i ) {
					if ( !m_bestBuyPrice.HasValue()
						 || it->second.Bids()[i].Price().Value()
								> m_bestBuyPrice.Value() ) {

						m_bestBuyPrice = it->second.Bids()[i].Price();
					}
				}

				for ( size_t i = 0; i < it->second.Asks().size(); ++i ) {
					if ( !m_bestSellPrice.HasValue()
						 || it->second.Asks()[i].Price().Value()
								< m_bestSellPrice.Value() ) {

						m_bestSellPrice = it->second.Asks()[i].Price();
					}
				}
			}
		} break;

		case zubr::ResponseType::ChannelPositions: {
			auto & r = static_cast<zubr::ChannelPositionsResponseWs &>( res );

			auto it = r.Entries().find( m_conf.InstrumentId() );

			if ( r.Entries().end() != it && INT_MAX == m_positionSize ) {
				m_positionSize = it->second.Size();
			}
		} break;

		case zubr::ResponseType::ChannelOrders: {
			auto & r = static_cast<zubr::ChannelOrdersResponseWs &>( res );

			//	std::cout << "orders"
			//			  << "\n";

			//	for ( size_t i = 0; i < r.Entries().size(); ++i ) {
			//		if ( r.Entries()[i].InstrumentId()
			//			 == zubrConf.InstrumentId() ) {

			//			std::cout << "\t\t";
			//			std::cout << "status: "
			//					  << static_cast<int>( r.Entries()[i].Status() )
			//					  << "\n";

			//			std::cout << "\t\t";
			//			std::cout << "price: " << r.Entries()[i].Price().Value()
			//					  << "\n";

			//			std::cout << "\t\t";
			//			std::cout
			//				<< "remaining: " <<
			// r.Entries()[i].QuantityRemaining()
			//				<< "\n";
			//		}
			//	}
		} break;
	}

	if ( !m_isBuyOrderPlaced && m_bestBuyPrice.HasValue()
		 && m_bestSellPrice.HasValue() && m_minPriceIncrement.HasValue()
		 && m_positionSize != INT_MAX ) {

		placeOrder( OrderDirection::Buy, m_isBuyOrderPlaced );
	}

	if ( !m_isSellOrderPlaced && m_bestBuyPrice.HasValue()
		 && m_bestSellPrice.HasValue() && m_minPriceIncrement.HasValue()
		 && m_positionSize != INT_MAX ) {

		placeOrder( OrderDirection::Sell, m_isSellOrderPlaced );
	}
}

void bot::start()
{
	m_connector.Start();
}

void bot::wait()
{
	m_connector.Wait();
}
