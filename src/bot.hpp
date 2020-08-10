/// bot.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBROBOT_BOT__H
#define __ZUBROBOT_BOT__H


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

		std::unordered_map<id_t, std::shared_ptr<PlaceOrderRequestWs>>
			m_orderReqMap;

		std::unordered_map<int64_t, std::shared_ptr<PlaceOrderRequestWs>>
			m_orderMap;

	protected:
		void connectHandler( zubr::AuthResponseWs & res );
		void messageHandler( zubr::ResponseWs & res );

		void placeOrder( OrderDirection direction, bool & isPlaced );

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
