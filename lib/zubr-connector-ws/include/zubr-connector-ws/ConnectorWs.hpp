/// ConnectorWs.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS__H
#define __ZUBR_CONNECTOR_WS__H


#include <atomic>
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

	/// @brief ZUBR websocket connector
	class ConnectorWs : public ConnectorBase {
	protected:
		std::string m_keyId;
		std::string m_keySecret;
		std::string m_endpoint;
		std::string m_hostname;

		const SerializerFactory & m_serializerFactory;

		websocketpp::client<websocketpp::config::asio_tls_client> m_client;
		websocketpp::client<
			websocketpp::config::asio_tls_client>::connection_ptr m_connection;

		std::thread m_clientThread;
		std::thread m_pingThread;
		std::atomic_flag m_isRunning;

		std::mutex m_sendSync;

		volatile id_t m_reqId;

		std::unordered_map<id_t, RequestWs> m_reqMap;
		std::mutex m_reqMapSync;

		std::function<void( ResponseWs & )> m_messageHandler;
		std::function<void( AuthResponseWs & )> m_connectHandler;

	protected:
		void OnWsOpen( websocketpp::connection_hdl hdl );
		void OnWsMessage( websocketpp::connection_hdl,
			websocketpp::client<
				websocketpp::config::asio_tls_client>::message_ptr msg );

		void OnWsFail( websocketpp::connection_hdl );

		websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
		OnWsTlsInit( const char * hostname, websocketpp::connection_hdl );

	public:
		/// @brief ZUBR websocket connector
		/// @param keyId API key ID
		/// @param keySecret API key secret
		/// @param endpoint API endpoint address
		/// @param hostname API endpoint hostname
		/// @return
		ConnectorWs( const std::string & keyId,
			const std::string & keySecret,
			const SerializerFactory & serializerFactory,
			const std::string & endpoint = "wss://uat.zubr.io/api/v1/ws",
			const std::string & hostname = "uat.zubr.io" )
			: m_keyId( keyId )
			, m_keySecret( keySecret )
			, m_serializerFactory( serializerFactory )
			, m_endpoint( endpoint )
			, m_hostname( hostname )
			, m_reqId( 0 )
		{
		}

		virtual ~ConnectorWs()
		{
			m_isRunning.clear();
		}

		/// @brief send request
		id_t Send( RequestWs & r );

		/// @brief send request
		/// @tparam TReq type of request
		/// @tparam ...TArgs
		/// @param ...args args for request
		/// @return request ID
		template <typename TReq, typename... TArgs>
		id_t Send( const TArgs &... args )
		{
			TReq req( args... );
			return Send( req );
		}

		/// @brief set connection handler (invoked on auth response message)
		/// @param handler
		void SetConnectHandler(
			const std::function<void( AuthResponseWs & )> & handler )
		{

			m_connectHandler = handler;
		}

		/// @brief set message handler (invoked on every incoming message)
		/// @param handler
		void SetMessageHandler(
			const std::function<void( ResponseWs & )> & handler )
		{

			m_messageHandler = handler;
		}

		/// @brief start client
		void Start() override;

		/// @brief wait for client termination
		void Wait();
	};

} // namespace zubr


#endif
