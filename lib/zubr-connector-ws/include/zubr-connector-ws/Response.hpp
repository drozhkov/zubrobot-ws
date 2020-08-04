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
		ChannelPositions
	};

	class ResponseWs : public Deserializable {
	protected:
		id_t m_id;
		bool m_isOk;
		ResponseType m_type;

	public:
		ResponseWs( ResponseType type = ResponseType::_undef )
			: m_type( type )
			, m_isOk( false )
			, m_id( -1 )
		{
		}

		static std::shared_ptr<ResponseWs> Deserialize( const std::string & in,
			const std::function<ResponseType( id_t id )> & typeResolver );

		void Deserialize( DeserializerWs * o ) override
		{
		}

		void Deserialize(
			const std::shared_ptr<ResponseWs> & out, DeserializerWs * o );

		ResponseType Type() const
		{
			return m_type;
		}

		int Id() const
		{
			return m_id;
		}

		bool IsOk() const
		{
			return m_isOk;
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

		void Deserialize( DeserializerWs * o ) override;

		int UserId() const
		{
			return m_userId;
		}
	};

	class OrderBookEntryItem : public Deserializable {
	protected:
		Number m_price;
		int m_quantity;

	public:
		void Deserialize( DeserializerWs * o ) override;

		const Number & Price() const
		{
			return m_price;
		}

		int Quantity() const
		{
			return m_quantity;
		}
	};

	class OrderBookEntry : public Deserializable {
	protected:
		int m_instrumentId;
		bool m_isSnapshot;

		std::vector<OrderBookEntryItem> m_bids;
		std::vector<OrderBookEntryItem> m_asks;

	public:
		void Deserialize( DeserializerWs * o ) override;

		int InstrumentId() const
		{
			return m_instrumentId;
		}

		const std::vector<OrderBookEntryItem> & Bids() const
		{
			return m_bids;
		}

		const std::vector<OrderBookEntryItem> & Asks() const
		{
			return m_asks;
		}
	};

	class OrderEntry : public Deserializable {
	protected:
		int m_instrumentId;
		OrderType m_type;
		OrderLifetime m_lifetime;
		OrderDirection m_direction;
		OrderStatus m_status;
		int m_quantityInitial;
		int m_quantityRemaining;
		Number m_price;

	public:
		void Deserialize( DeserializerWs * o ) override;

		int InstrumentId() const
		{
			return m_instrumentId;
		}

		OrderType Type() const
		{
			return m_type;
		}

		OrderLifetime Lifetime() const
		{
			return m_lifetime;
		}

		OrderDirection Direction() const
		{
			return m_direction;
		}

		OrderStatus Status() const
		{
			return m_status;
		}

		int QuantityInitial() const
		{
			return m_quantityInitial;
		}

		int QuantityRemaining() const
		{
			return m_quantityRemaining;
		}

		const Number & Price() const
		{
			return m_price;
		}
	};

	class ChannelOrdersResponseWs : public ResponseWs {
	protected:
		std::vector<OrderEntry> m_entries;

	public:
		ChannelOrdersResponseWs()
			: ResponseWs( ResponseType::ChannelOrders )
		{
		}

		void Deserialize( DeserializerWs * o ) override;

		const std::vector<OrderEntry> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelOrderBookResponseWs : public ResponseWs {
	protected:
		std::vector<OrderBookEntry> m_entries;

	public:
		ChannelOrderBookResponseWs()
			: ResponseWs( ResponseType::ChannelOrderBook )
		{
		}

		void Deserialize( DeserializerWs * o ) override;

		const std::vector<OrderBookEntry> & Entries() const
		{
			return m_entries;
		}
	};

	class ChannelPositionsResponseWs : public ResponseWs {
	protected:
		int m_instrumentId;
		int m_quantity;

		Number m_unrealizedPnl;
		Number m_realizedPnl;
		Number m_margin;
		Number m_maxRemovableMargin;
		Number m_entryPrice;
		Number m_entryNotionalValue;
		Number m_currentNotionalValue;
		Number m_partialLiquidationPrice;
		Number m_fullLiquidationPrice;

	public:
		ChannelPositionsResponseWs()
			: ResponseWs( ResponseType::ChannelPositions )
		{
		}

		void Deserialize( DeserializerWs * o ) override;

		int InstrumentId() const
		{
			return m_instrumentId;
		}

		const Number & UnrealizedPnl() const
		{
			return m_unrealizedPnl;
		}

		const Number & RealizedPnl() const
		{
			return m_realizedPnl;
		}

		const Number & Margin() const
		{
			return m_margin;
		}

		const Number & MaxRemovableMargin() const
		{
			return m_maxRemovableMargin;
		}
	};

} // namespace zubr


#endif
