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

/// Response.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_RESPONSE__H
#define __ZUBR_CONNECTOR_WS_RESPONSE__H


#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Types.hpp"


namespace zubr {

	enum class ResponseType {
		_undef = 0,
		Auth,
		PlaceOrder,
		ChannelOrders,
		ChannelOrderBook,
		ChannelPositions,
		ChannelInstruments
	};

	class ResponseWs : public Serializable {
	protected:
		t_req_id m_id;
		bool m_isOk;
		ResponseType m_type;
		std::string m_errorCodeName;

	public:
		ResponseWs( ResponseType type = ResponseType::_undef )
			: m_type( type )
			, m_isOk( false )
			, m_id( -1 )
		{
		}

		static std::shared_ptr<ResponseWs> Deserialize( Serializer & s,
			const std::string & in,
			const std::function<ResponseType( t_req_id id )> & typeResolver );

		void Deserialize( Serializer & s ) override
		{
		}

		void Deserialize(
			const std::shared_ptr<ResponseWs> & out, Serializer & s );

		ResponseType Type() const
		{
			return m_type;
		}

		t_req_id Id() const
		{
			return m_id;
		}

		/// @brief is valid response
		/// @return
		bool IsOk() const
		{
			return m_isOk;
		}

		const std::string & ErrorCodeName() const
		{
			return m_errorCodeName;
		}
	};

	/// @brief authentication response
	class AuthResponseWs : public ResponseWs {
	protected:
		int m_userId;

	public:
		AuthResponseWs()
			: ResponseWs( ResponseType::Auth )
		{
		}

		void Deserialize( Serializer & s ) override;

		int UserId() const
		{
			return m_userId;
		}
	};

	class PlaceOrderResponseWs : public ResponseWs {
	protected:
		t_order_id m_orderId;

	public:
		PlaceOrderResponseWs()
			: ResponseWs( ResponseType::PlaceOrder )
		{
		}

		void Deserialize( Serializer & s ) override;

		t_order_id OrderId() const
		{
			return m_orderId;
		}
	};

	class ChannelOrdersResponseWs : public ResponseWs {
	protected:
		std::unordered_map<t_order_id, OrderEntry> m_entries;

	public:
		ChannelOrdersResponseWs()
			: ResponseWs( ResponseType::ChannelOrders )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<t_order_id, OrderEntry> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelOrderBookResponseWs : public ResponseWs {
	protected:
		std::unordered_map<t_instrument_id, OrderBookEntry> m_entries;

	public:
		ChannelOrderBookResponseWs()
			: ResponseWs( ResponseType::ChannelOrderBook )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<t_instrument_id, OrderBookEntry> &
		Entries() const
		{
			return m_entries;
		}
	};

	class ChannelPositionsResponseWs : public ResponseWs {
	protected:
		std::unordered_map<t_instrument_id, Position> m_entries;

	public:
		ChannelPositionsResponseWs()
			: ResponseWs( ResponseType::ChannelPositions )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<t_instrument_id, Position> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelInstrumentsResponseWs : public ResponseWs {
	protected:
		std::unordered_map<t_instrument_id, Instrument> m_list;

	public:
		ChannelInstrumentsResponseWs()
			: ResponseWs( ResponseType::ChannelInstruments )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<t_instrument_id, Instrument> & List() const
		{
			return m_list;
		}
	};

} // namespace zubr


#endif
