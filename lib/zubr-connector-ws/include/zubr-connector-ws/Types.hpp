/// Types.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_TYPES__H
#define __ZUBR_CONNECTOR_WS_TYPES__H


namespace zubr {

	enum class OrderDirection { _undef = 0, Buy, Sell };
	enum class OrderType { _undef = 0, Limit, PostOnly };
	enum class OrderLifetime { _undef = 0, Gtc, IoC, FoK };

	class OrderEnumHelper {
	public:
		static const char * ToString( OrderDirection direction )
		{
			switch ( direction ) {
				case OrderDirection::Buy:
					return "BUY";
					break;

				case OrderDirection::Sell:
					return "SELL";
					break;

				default:
					return "N/A";
			}
		}

		static const char * ToString( OrderType type )
		{
			switch ( type ) {
				case OrderType::Limit:
					return "LIMIT";
					break;

				case OrderType::PostOnly:
					return "POST_ONLY";
					break;

				default:
					return "N/A";
			}
		}

		static const char * ToString( OrderLifetime lifetime )
		{
			switch ( lifetime ) {
				case OrderLifetime::FoK:
					return "FOK";
					break;

				case OrderLifetime::Gtc:
					return "GTC";
					break;

				case OrderLifetime::IoC:
					return "IOC";
					break;

				default:
					return "N/A";
			}
		}
	};

	struct SerializerWs;
	struct DeserializerWs;

	class Serializable {
	public:
		virtual void Serialize( SerializerWs * o )
		{
		}
	};

	class Deserializable {
	public:
		virtual void Deserialize( DeserializerWs * o )
		{
		}
	};

	class Number : public Serializable, public Deserializable {
	protected:
		int m_significand;
		int m_exponent;

	public:
		Number( int significand, int exponent )
			: m_significand( significand )
			, m_exponent( exponent )
		{
		}

		void Serialize( SerializerWs * o ) override;
		void Deserialize( DeserializerWs * o ) override;
	};

} // namespace zubr


#endif
