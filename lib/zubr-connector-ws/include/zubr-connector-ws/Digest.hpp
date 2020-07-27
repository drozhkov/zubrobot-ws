/// Digest.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_CONNECTOR_WS_DIGEST__H
#define __ZUBR_CONNECTOR_WS_DIGEST__H


#include <string>
#include <vector>


namespace zubr {

	class Digest {
	public:
		static void FromHex(
			std::vector<uint8_t> & out, const std::string & hex );

		static void ToHex(
			std::string & out, uint8_t * buffer, size_t bufferLen );

		static void Calculate( std::string & out,
			const std::string & keyId,
			const std::string & keySecretHex,
			uint64_t ts );
	};

} // namespace zubr


#endif
