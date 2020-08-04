/// ConnectorWs.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <limits>

#include "../include/zubr-connector-ws/Request.hpp"
#include "../include/zubr-connector-ws/Response.hpp"

#include "../include/zubr-connector-ws/ConnectorWs.hpp"


//#define ZUBR_DEBUG


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

#ifdef ZUBR_DEBUG
	std::cout << payload << std::endl;
#endif

	auto res = ResponseWs::Deserialize( payload, [this]( id_t id ) {
		const std::lock_guard<std::mutex> lock( m_reqMapSync );
		auto reqMapIt = m_reqMap.find( id );

		if ( m_reqMap.end() != reqMapIt ) {
			if ( reqMapIt->second.MethodName()
				 == AuthRequestWs::ReqMethodName ) {

				return ResponseType::Auth;
			}

			if ( reqMapIt->second.MethodName()
				 == PlaceOrderRequestWs::ReqMethodName ) {

				return ResponseType::PlaceOrder;
			}

			m_reqMap.erase( id );
		}

		return ResponseType::_undef;
	} );

	if ( res->Type() == ResponseType::Auth ) {
#ifdef ZUBR_DEBUG
		std::cout << "auth result: " << ( res->IsOk() ? "ok" : "err" )
				  << std::endl;
#endif

		if ( m_connectHandler ) {
			m_connectHandler( *static_cast<AuthResponseWs *>( res.get() ) );
		}
	}

	if ( m_messageHandler ) {
		m_messageHandler( *res );
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

zubr::id_t ConnectorWs::Send( RequestWs & r )
{
	const std::lock_guard<std::mutex> lock( m_sendSync );

	r.Id( ++m_reqId );
	id_t result = m_reqId;

	{
		const std::lock_guard<std::mutex> lock( m_reqMapSync );
		m_reqMap[result] = r;
	}

	if ( m_reqId == std::numeric_limits<id_t>::max() ) {
		m_reqId = 0;
	}

	std::string req;
	RequestWs::Serialize( req, r );

#ifdef ZUBR_DEBUG
	std::cout << req << std::endl;
#endif

	std::error_code ec;
	m_client.send(
		m_connection->get_handle(), req, websocketpp::frame::opcode::text, ec );

	return result;
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

		m_isRunning.test_and_set();

		m_clientThread = std::thread( [this] {
			while ( m_isRunning.test_and_set() ) {
				websocketpp::lib::error_code ec;

				{
					const std::lock_guard<std::mutex> lock( m_sendSync );

					m_connection = m_client.get_connection( m_endpoint, ec );

					if ( !m_connection || ec ) {
#ifdef ZUBR_DEBUG
						std::cout
							<< "could not create connection: " << ec.message()
							<< std::endl;
#endif

						std::this_thread::sleep_for(
							std::chrono::seconds( 2 ) );

						continue;
					}

					m_client.connect( m_connection );
				}

				m_client.run();
			}
		} );
	}
	catch ( websocketpp::exception const & e ) {
		std::cout << e.what() << std::endl;
	}
}

void ConnectorWs::Wait()
{
	if ( m_clientThread.joinable() ) {
		m_clientThread.join();
	}
}
