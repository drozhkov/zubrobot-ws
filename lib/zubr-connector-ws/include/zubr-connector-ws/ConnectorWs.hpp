/// ConnectorWs.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS__H
#define __ZUBR_CONNECTOR_WS__H


#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"

#include "zubr-core/ConnectorBase.hpp"

#include "Request.hpp"
#include "Response.hpp"


namespace zubr {

	class ConnectorWs : public ConnectorBase {
	protected:
		std::string m_keyId;
		std::string m_keySecret;
		std::string m_endpoint;
		std::string m_hostname;

		websocketpp::client<websocketpp::config::asio_tls_client> m_client;
		websocketpp::client<
			websocketpp::config::asio_tls_client>::connection_ptr m_connection;

		std::thread m_clientThread;

		std::mutex m_sendSync;

		int m_reqId;

		std::unordered_map<int, RequestWs> m_reqMap;
		std::mutex m_reqMapSync;

		std::function<void( ResponseWs & )> m_messageHandler;

	protected:
		void OnWsOpen( websocketpp::connection_hdl hdl );
		void OnWsMessage( websocketpp::connection_hdl,
			websocketpp::client<
				websocketpp::config::asio_tls_client>::message_ptr msg );

		websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
		OnWsTlsInit( const char * hostname, websocketpp::connection_hdl );

		void Send( RequestWs & r );

	public:
		/// @brief Zubr websocket connector
		/// @param keyId API key ID
		/// @param keySecret API key secret
		/// @param endpoint API endpoint address
		/// @param hostname API endpoint hostname
		/// @return
		ConnectorWs( const std::string & keyId,
			const std::string & keySecret,
			const std::string & endpoint = "wss://uat.zubr.io/api/v1/ws",
			const std::string & hostname = "uat.zubr.io" )
			: m_keyId( keyId )
			, m_keySecret( keySecret )
			, m_endpoint( endpoint )
			, m_hostname( hostname )
			, m_reqId( 0 )
		{
		}

		template <typename TReq, typename... TArgs>
		void Send( const TArgs &... args )
		{
			TReq req( args... );
			Send( req );
		}

		void SetMessageHandler(
			const std::function<void( ResponseWs & )> & handler )
		{
			m_messageHandler = handler;
		}

		void Start() override;
	};

} // namespace zubr


#endif
