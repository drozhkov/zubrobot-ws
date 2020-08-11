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

/// bot.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBROBOT_BOT__H
#define __ZUBROBOT_BOT__H


#include <set>
#include <unordered_map>

#include "zubr-core/JsonSerializer.hpp"

#include "zubr-connector-ws/ConnectorWs.hpp"

#include "conf.hpp"


namespace zubr {

	class bot {
	protected:
		conf m_conf;

		JsonSerializerFactory m_serializerFactory;

		ConnectorWs m_connector;

		Number m_minPriceIncrement;
		Number m_bestBuyPrice;
		Number m_bestSellPrice;
		int m_positionSize;

		bool m_isBuyOrderPlaced;
		bool m_isSellOrderPlaced;

		std::unordered_map<t_req_id, std::shared_ptr<PlaceOrderRequestWs>>
			m_orderReqMap;

		std::unordered_map<t_order_id, std::shared_ptr<PlaceOrderRequestWs>>
			m_sellOrdersMap;

		std::unordered_map<t_order_id, std::shared_ptr<PlaceOrderRequestWs>>
			m_buyOrdersMap;

		std::set<Number> m_orderBookBid;
		std::set<Number> m_orderBookAsk;

	protected:
		Number calculateOrderPrice( OrderDirection direction );
		void placeOrder(
			OrderDirection direction, int quantity, bool & isPlaced );

		void replaceOrderIfPriceChanged( OrderDirection direction );

		void orderUpdateHandler( const OrderEntry & order );

		void connectHandler( zubr::AuthResponseWs & res );

		/// @brief main handler for incoming messages
		/// @param res
		void messageHandler( zubr::ResponseWs & res );

	public:
		bot( const conf & conf )
			: m_conf( conf )
			, m_serializerFactory()
			, m_connector( conf.Api().KeyId(),
				  conf.Api().KeySecret(),
				  m_serializerFactory,
				  conf.Api().Url(),
				  conf.Api().Host() )
			, m_isBuyOrderPlaced( false )
			, m_isSellOrderPlaced( false )
		{

			m_positionSize = conf.UseConfigStartPositionSize()
								 ? conf.PositionSizeStart()
								 : INT_MAX;

			m_connector.SetConnectHandler( std::bind(
				&bot::connectHandler, this, std::placeholders::_1 ) );

			m_connector.SetMessageHandler( std::bind(
				&bot::messageHandler, this, std::placeholders::_1 ) );
		}

		void start();
		void wait();
	};

} // namespace zubr


#endif
