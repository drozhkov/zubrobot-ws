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

/// bot.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "zubr-core/Logger.hpp"

#include "bot.hpp"


using namespace zubr;


Number bot::calculateOrderPrice( OrderDirection direction )
{
	zubr::Number price = m_bestBuyPrice;
	price.Add( m_bestSellPrice ).Div( 2 );

	// BUY price = (current best purchase price + current best sale
	// price) / 2 -
	//	interest - shift * position;
	// SELL price = (current best purchase price +
	//	current best sale price) / 2 + interest - shift * position.
	if ( OrderDirection::Buy == direction ) {
		price.Sub( m_conf.Interest() );
	}
	else {
		price.Add( m_conf.Interest() );
	}

	price.Sub( m_conf.Shift() * m_positionSize ).ModRing( m_minPriceIncrement );

	return price;
}

void bot::placeOrder( OrderDirection direction, int quantity, bool & isPlaced )
{
	zubr::Number price = calculateOrderPrice( direction );

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

void bot::replaceOrderIfPriceChanged( OrderDirection direction )
{
	std::unordered_map<t_order_id, std::shared_ptr<PlaceOrderRequestWs>> &
		ordersMap
		= OrderDirection::Buy == direction ? m_buyOrdersMap : m_sellOrdersMap;

	if ( !ordersMap.empty() ) {
		auto price = calculateOrderPrice( direction );

		for ( auto & itOrder : ordersMap ) {
			// only less (<) operator is implemented
			if ( itOrder.second->Price() < price
				 || price < itOrder.second->Price() ) {

				ZUBR_LOG_INFO( "replacing order... old price: "
							   << itOrder.second->Price().Value()
							   << ", new price: " << price.Value() );

				m_connector.Send<CancelOrderRequestWs>( itOrder.first );

				itOrder.second->Price( price );
				itOrder.second->IsReplaceOrder( true );
			}
		}
	}
}

void bot::orderUpdateHandler( const OrderEntry & order )
{

	std::unordered_map<t_order_id, std::shared_ptr<PlaceOrderRequestWs>> &
		ordersMap
		= order.Direction() == OrderDirection::Buy ? m_buyOrdersMap
												   : m_sellOrdersMap;

	bool & isOrderPlaced = order.Direction() == OrderDirection::Buy
							   ? m_isBuyOrderPlaced
							   : m_isSellOrderPlaced;

	auto itOrderMap = ordersMap.find( order.Id() );

	if ( ordersMap.end() != itOrderMap ) {
		if ( order.Status() == OrderStatus::Filled
			 || order.Status() == OrderStatus::PartiallyFilled ) {

			auto ordersFilledCount
				= itOrderMap->second->Quantity() - order.QuantityRemaining();

			if ( order.Direction() == OrderDirection::Buy ) {
				m_positionSize += ordersFilledCount;
			}
			else if ( order.Direction() == OrderDirection::Sell ) {
				m_positionSize -= ordersFilledCount;
			}

			itOrderMap->second->Quantity( order.QuantityRemaining() );

			if ( order.QuantityRemaining() == 0 ) {
				ordersMap.erase( order.Id() );
				isOrderPlaced = false;
			}
		}
		else if ( order.Status() == OrderStatus::Cancelled ) {
			if ( itOrderMap->second->IsReplaceOrder()
				 && itOrderMap->second->Quantity() > 0 ) {

				placeOrder( itOrderMap->second->Direction(),
					itOrderMap->second->Quantity(),
					itOrderMap->second->Direction() == OrderDirection::Buy
						? m_isBuyOrderPlaced
						: m_isSellOrderPlaced );
			}
			else {
				isOrderPlaced = false;
			}

			ordersMap.erase( order.Id() );
		}
	}
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
				if ( req->Direction() == OrderDirection::Buy ) {
					m_buyOrdersMap[r.OrderId()] = req;
				}
				else if ( req->Direction() == OrderDirection::Sell ) {
					m_sellOrdersMap[r.OrderId()] = req;
				}
			}

			m_orderReqMap.erase( r.Id() );
		} break;

		case zubr::ResponseType::ChannelInstruments: {
			auto & r = static_cast<zubr::ChannelInstrumentsResponseWs &>( res );
			auto it = r.List().find( m_conf.InstrumentId() );

			if ( r.List().end() != it ) {
				m_minPriceIncrement = it->second.MinPriceIncrement();
			}
		} break;

		case zubr::ResponseType::ChannelOrderBook: {
			auto & r = static_cast<zubr::ChannelOrderBookResponseWs &>( res );
			auto it = r.Entries().find( m_conf.InstrumentId() );

			if ( r.Entries().end() != it ) {
				for ( size_t i = 0; i < it->second.Bids().size(); ++i ) {
					auto & price = it->second.Bids()[i].Price();

					if ( it->second.Bids()[i].Quantity() > 0 ) {
						m_orderBookBid.insert( price );
					}
					else {
						m_orderBookBid.erase( price );
					}
				}

				for ( size_t i = 0; i < it->second.Asks().size(); ++i ) {
					auto & price = it->second.Asks()[i].Price();

					if ( it->second.Asks()[i].Quantity() > 0 ) {
						m_orderBookAsk.insert( price );
					}
					else {
						m_orderBookAsk.erase( price );
					}
				}

				if ( !m_orderBookBid.empty() ) {
					m_bestBuyPrice = *( --m_orderBookBid.end() );
				}

				if ( !m_orderBookAsk.empty() ) {
					m_bestSellPrice = *( m_orderBookAsk.begin() );
				}

				ZUBR_LOG_INFO( "best BUY price: " << m_bestBuyPrice.Value()
												  << "\tbest SELL price: "
												  << m_bestSellPrice.Value() );
			}
		} break;

		case zubr::ResponseType::ChannelPositions: {
			auto & r = static_cast<zubr::ChannelPositionsResponseWs &>( res );

			if ( INT_MAX == m_positionSize ) {
				auto it = r.Entries().find( m_conf.InstrumentId() );
				m_positionSize
					= r.Entries().end() != it ? it->second.Size() : 0;

				ZUBR_LOG_INFO( "exchange position size: " << m_positionSize );
			}
		} break;

		case zubr::ResponseType::ChannelOrders: {
			auto & r = static_cast<zubr::ChannelOrdersResponseWs &>( res );

			for ( auto & itOrder : r.Entries() ) {
				orderUpdateHandler( itOrder.second );
			}
		} break;
	}

	if ( m_bestBuyPrice.HasValue() && m_bestSellPrice.HasValue()
		 && m_minPriceIncrement.HasValue() && m_positionSize != INT_MAX ) {

		if ( !m_isBuyOrderPlaced && m_positionSize < m_conf.PositionSizeMax()
			 && m_buyOrdersMap.empty() ) {

			placeOrder(
				OrderDirection::Buy, m_conf.Quantity(), m_isBuyOrderPlaced );
		}

		if ( !m_isSellOrderPlaced && m_positionSize > -m_conf.PositionSizeMax()
			 && m_sellOrdersMap.empty() ) {

			placeOrder(
				OrderDirection::Sell, m_conf.Quantity(), m_isSellOrderPlaced );
		}

		ZUBR_LOG_INFO( "position size: " << m_positionSize );

		replaceOrderIfPriceChanged( OrderDirection::Buy );
		replaceOrderIfPriceChanged( OrderDirection::Sell );
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
