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

	typedef int64_t id_t;

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
		id_t m_id;
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
			const std::function<ResponseType( id_t id )> & typeResolver );

		void Deserialize( Serializer & s ) override
		{
		}

		void Deserialize(
			const std::shared_ptr<ResponseWs> & out, Serializer & s );

		ResponseType Type() const
		{
			return m_type;
		}

		int Id() const
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
		int64_t m_orderId;

	public:
		PlaceOrderResponseWs()
			: ResponseWs( ResponseType::PlaceOrder )
		{
		}

		void Deserialize( Serializer & s ) override;

		int64_t OrderId() const
		{
			return m_orderId;
		}
	};

	class ChannelOrdersResponseWs : public ResponseWs {
	protected:
		std::unordered_map<int, OrderEntry> m_entries;

	public:
		ChannelOrdersResponseWs()
			: ResponseWs( ResponseType::ChannelOrders )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<int, OrderEntry> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelOrderBookResponseWs : public ResponseWs {
	protected:
		std::unordered_map<int, OrderBookEntry> m_entries;

	public:
		ChannelOrderBookResponseWs()
			: ResponseWs( ResponseType::ChannelOrderBook )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<int, OrderBookEntry> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelPositionsResponseWs : public ResponseWs {
	protected:
		std::unordered_map<int, Position> m_entries;

	public:
		ChannelPositionsResponseWs()
			: ResponseWs( ResponseType::ChannelPositions )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<int, Position> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelInstrumentsResponseWs : public ResponseWs {
	protected:
		std::unordered_map<int, Instrument> m_list;

	public:
		ChannelInstrumentsResponseWs()
			: ResponseWs( ResponseType::ChannelInstruments )
		{
		}

		void Deserialize( Serializer & s ) override;

		const std::unordered_map<int, Instrument> & List() const
		{
			return m_list;
		}
	};

} // namespace zubr


#endif
