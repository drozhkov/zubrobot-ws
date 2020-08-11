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

/// ConnectorWs.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <limits>

#include "zubr-core/Logger.hpp"

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

void ConnectorWs::OnWsMessage( websocketpp::connection_hdl hdl,
	websocketpp::client<websocketpp::config::asio_tls_client>::message_ptr msg )
{
	auto & payload = msg->get_payload();

	ZUBR_LOG_DEBUG( payload );

	auto res = ResponseWs::Deserialize(
		*m_serializerFactory.Create(), payload, [this]( t_req_id id ) {
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
		if ( m_connectHandler ) {
			m_connectHandler( *static_cast<AuthResponseWs *>( res.get() ) );

			if ( !res->IsOk() ) {
				ZUBR_LOG_ERROR( "authentication failed" );
				m_isRunning.clear();

				websocketpp::lib::error_code ec;
				m_client.close(
					hdl, websocketpp::close::status::normal, "foo", ec );
			}
		}
	}

	if ( m_messageHandler ) {
		m_messageHandler( *res );
	}
}

void ConnectorWs::OnWsFail( websocketpp::connection_hdl )
{
	ZUBR_LOG_ERROR( "ConnectorWs::OnWsFail" );
	m_isRunning.clear();
	m_client.stop();
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

zubr::t_req_id ConnectorWs::Send( RequestWs & r )
{
	const std::lock_guard<std::mutex> lock( m_sendSync );

	r.Id( ++m_reqId );
	t_req_id result = m_reqId;

	{
		const std::lock_guard<std::mutex> lock( m_reqMapSync );
		m_reqMap[result] = r;
	}

	if ( m_reqId == std::numeric_limits<t_req_id>::max() ) {
		m_reqId = 0;
	}

	std::string req;
	RequestWs::Serialize( req, *m_serializerFactory.Create(), r );

	ZUBR_LOG_DEBUG( req );

	std::error_code ec;
	m_client.send(
		m_connection->get_handle(), req, websocketpp::frame::opcode::text, ec );

	return result;
}

void ConnectorWs::Start()
{
	try {
		m_client.set_access_channels( websocketpp::log::alevel::none );
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

		m_client.set_fail_handler(
			websocketpp::lib::bind( &ConnectorWs::OnWsFail,
				this,
				websocketpp::lib::placeholders::_1 ) );

		m_isRunning.test_and_set();

		m_clientThread = std::thread( [this] {
			while ( m_isRunning.test_and_set() ) {
				websocketpp::lib::error_code ec;

				{
					const std::lock_guard<std::mutex> lock( m_sendSync );

					m_connection = m_client.get_connection( m_endpoint, ec );

					if ( !m_connection || ec ) {
						ZUBR_LOG_ERROR( ec.message() );

						std::this_thread::sleep_for(
							std::chrono::seconds( 2 ) );

						continue;
					}

					m_client.connect( m_connection );
				}

				m_client.run();
			}

			m_isRunning.clear();
		} );

		m_pingThread = std::thread( [this] {
			while ( m_isRunning.test_and_set() ) {
				std::this_thread::sleep_for( std::chrono::seconds( 14 ) );

				{
					const std::lock_guard<std::mutex> lock( m_sendSync );
					m_client.ping( m_connection, "zubrobot-ws" );
				}
			}

			m_isRunning.clear();
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
