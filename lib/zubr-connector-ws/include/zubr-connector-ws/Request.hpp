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

/// Request.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_REQUEST__H
#define __ZUBR_CONNECTOR_WS_REQUEST__H


#include <memory>
#include <string>

#include "zubr-core/Serializer.hpp"

#include "Types.hpp"


namespace zubr {

	/// @brief base class for requests
	class RequestWs : public Serializable {
	protected:
		const static int MethodIdRequest = 9;
		const static int MethodIdSubscribe = 1;
		const static int MethodIdUnsubscribe = 2;

	protected:
		int m_id;
		int m_methodId;
		std::string m_methodName;
		Channel m_channel;

	public:
		RequestWs( const std::string & methodName = "",
			int methodId = MethodIdRequest,
			Channel channel = Channel::_undef )
			: m_methodName( methodName )
			, m_methodId( methodId )
			, m_channel( channel )
			, m_id( 0 )
		{
		}

		static void Serialize(
			std::string & out, Serializer & s, RequestWs & req );

		void Serialize( Serializer & s ) override
		{
		}

		void SerializeBase( Serializer & s );

		/// @brief set request ID
		/// @param id
		void Id( int id )
		{
			m_id = id;
		}

		/// @brief get request ID
		/// @return
		int Id() const
		{
			return m_id;
		}

		/// @brief get method name
		/// @return
		const std::string & MethodName() const
		{
			return m_methodName;
		}
	};

	/// @brief authentication request
	class AuthRequestWs : public RequestWs {
	protected:
		std::string m_keyId;
		std::string m_keySecret;

	public:
		const static std::string ReqMethodName;

	public:
		/// @brief authentication request
		/// @param keyId
		/// @param keySecret
		/// @return
		AuthRequestWs(
			const std::string & keyId, const std::string & keySecret )
			: RequestWs( ReqMethodName )
			, m_keyId( keyId )
			, m_keySecret( keySecret )
		{
		}

		void Serialize( Serializer & s ) override;
	};

	/// @brief place order request
	class PlaceOrderRequestWs : public RequestWs {
	protected:
		int m_instrumentId;
		Number m_price;
		OrderDirection m_direction;
		int m_quantity;
		OrderType m_type;
		OrderLifetime m_lifetime;
		bool m_isReplaceOrder;

	public:
		const static std::string ReqMethodName;

	public:
		/// @brief place order request
		/// @param instrumentId Instrument number
		/// @param price
		/// @param direction Order direction: 'BUY' or 'SELL'
		/// @param quantity Order size quantity
		/// @param type Order type: 'LIMIT' or 'POST_ONLY'
		/// @param lifetime Order lifetime parameter. Has three possible
		/// options:
		///		'GTC' - good 'til cancelled order, active while not cancelled or
		///			executed
		///		'IOC' - immediate or cancel order, unfilled part or
		///			order immediately canceled
		///		'FOK' - fill or kill order, complete
		///			execution or nothing
		/// @return
		PlaceOrderRequestWs( int instrumentId,
			const Number & price,
			OrderDirection direction,
			int quantity,
			OrderType type,
			OrderLifetime lifetime )
			: RequestWs( ReqMethodName )
			, m_instrumentId( instrumentId )
			, m_price( price )
			, m_direction( direction )
			, m_quantity( quantity )
			, m_type( type )
			, m_lifetime( lifetime )
			, m_isReplaceOrder( false )
		{
		}

		void Serialize( Serializer & s ) override;

		OrderDirection Direction() const
		{
			return m_direction;
		}

		int Quantity() const
		{
			return m_quantity;
		}

		void Quantity( int v )
		{
			m_quantity = v;
		}

		const Number & Price() const
		{
			return m_price;
		}

		void Price( const Number & v )
		{
			m_price = v;
		}

		bool IsReplaceOrder() const
		{
			return m_isReplaceOrder;
		}

		void IsReplaceOrder( bool v )
		{
			m_isReplaceOrder = v;
		}
	};

	class ReplaceOrderRequestWs : public RequestWs {
	protected:
		t_order_id m_orderId;
		Number m_price;
		int m_quantity;

	public:
		const static std::string ReqMethodName;

	public:
		/// @brief replace order request
		ReplaceOrderRequestWs(
			t_order_id orderId, const Number & price, int quantity )
			: RequestWs( ReqMethodName )
			, m_orderId( orderId )
			, m_price( price )
			, m_quantity( quantity )
		{
		}

		void Serialize( Serializer & s ) override;
	};

	class CancelOrderRequestWs : public RequestWs {
	protected:
		t_order_id m_orderId;

	public:
		const static std::string ReqMethodName;

	public:
		/// @brief cancel order request
		CancelOrderRequestWs( t_order_id orderId )
			: RequestWs( ReqMethodName )
			, m_orderId( orderId )
		{
		}

		void Serialize( Serializer & s ) override;
	};

	/// @brief channel subscription request
	class SubscribeRequestWs : public RequestWs {
	public:
		SubscribeRequestWs( Channel channel )
			: RequestWs( "", MethodIdSubscribe, channel )
		{
		}
	};

} // namespace zubr


#endif
