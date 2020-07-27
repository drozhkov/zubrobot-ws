/// ConnectorWs.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <limits>

#include "../include/zubr-connector-ws/Request.hpp"
#include "../include/zubr-connector-ws/Response.hpp"

#include "../include/zubr-connector-ws/ConnectorWs.hpp"


using namespace zubr;


void ConnectorWs::OnWsOpen( websocketpp::connection_hdl hdl )
{
	m_client.get_alog().write(
		websocketpp::log::alevel::app, "Connection opened" );

	Send<AuthRequestWs>( m_keyId, m_keySecret );
}

void ConnectorWs::OnWsMessage( websocketpp::connection_hdl,
	websocketpp::client<websocketpp::config::asio_tls_client>::message_ptr msg )
{
	auto & payload = msg->get_payload();

#if 1
	std::cout << payload << std::endl;
#endif

	ResponseWs r( ResponseType::_undef );
	ResponseWs::Deserialize( r, payload );

	if ( r.Id() > -1 ) {
		const std::lock_guard<std::mutex> lock( m_reqMapSync );
		auto reqMapIt = m_reqMap.find( r.Id() );

		if ( m_reqMap.end() != reqMapIt ) {
			ResponseWs res( ResponseType::_undef );

			if ( reqMapIt->second.MethodName()
				 == AuthRequestWs::ReqMethodName ) {

				res = AuthResponseWs();
				ResponseWs::Deserialize( res, payload );

				std::cout << "auth result: " << ( res.IsOk() ? "ok" : "err" )
						  << std::endl;
			}

			m_reqMap.erase( r.Id() );

			if ( m_messageHandler ) {
				m_messageHandler( res );
			}
		}
	}
}

websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
ConnectorWs::OnWsTlsInit( const char * hostname, websocketpp::connection_hdl )
{
	websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> ctx
		= websocketpp::lib::make_shared<boost::asio::ssl::context>(
			boost::asio::ssl::context::sslv23 );

	try {
		ctx->set_options( boost::asio::ssl::context::default_workarounds
						  | boost::asio::ssl::context::no_sslv2
						  | boost::asio::ssl::context::no_sslv3
						  | boost::asio::ssl::context::single_dh_use );

		ctx->set_verify_mode( boost::asio::ssl::verify_none );
	}
	catch ( std::exception & e ) {
		std::cout << e.what() << std::endl;
	}

	return ctx;
}

void ConnectorWs::Send( RequestWs & r )
{
	const std::lock_guard<std::mutex> lock( m_sendSync );

	r.Id( ++m_reqId );

	{
		const std::lock_guard<std::mutex> lock( m_reqMapSync );
		m_reqMap[m_reqId] = r;
	}

	if ( m_reqId == std::numeric_limits<int>::max() ) {
		m_reqId = 0;
	}

	std::string req;
	RequestWs::Serialize( req, r );

#if 1
	std::cout << req << std::endl;
#endif

	m_client.send(
		m_connection->get_handle(), req, websocketpp::frame::opcode::text );
}

void ConnectorWs::Start()
{
	try {
		m_client.set_access_channels( websocketpp::log::alevel::all );
		m_client.clear_access_channels(
			websocketpp::log::alevel::frame_payload );
		m_client.set_error_channels( websocketpp::log::elevel::all );

		m_client.init_asio();

		m_client.set_open_handler(
			websocketpp::lib::bind( &ConnectorWs::OnWsOpen,
				this,
				websocketpp::lib::placeholders::_1 ) );

		m_client.set_message_handler(
			websocketpp::lib::bind( &ConnectorWs::OnWsMessage,
				this,
				websocketpp::lib::placeholders::_1,
				websocketpp::lib::placeholders::_2 ) );

		m_client.set_tls_init_handler(
			websocketpp::lib::bind( &ConnectorWs::OnWsTlsInit,
				this,
				m_hostname.c_str(),
				websocketpp::lib::placeholders::_1 ) );

		websocketpp::lib::error_code ec;
		m_connection = m_client.get_connection( m_endpoint, ec );

		if ( ec ) {
			std::cout << "could not create connection: " << ec.message()
					  << std::endl;

			return;
		}

		m_client.connect( m_connection );
		m_clientThread = std::thread( [this] { m_client.run(); } );
	}
	catch ( websocketpp::exception const & e ) {
		std::cout << e.what() << std::endl;
	}
}
