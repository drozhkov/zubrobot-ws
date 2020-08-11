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

/// Digest.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <iostream>
#include <sstream>

#include "openssl/hmac.h"
#include "openssl/sha.h"

#include "../include/zubr-connector-ws/Digest.hpp"


using namespace zubr;


void Digest::FromHex( std::vector<uint8_t> & out, const std::string & hex )
{
	std::string token;

	if ( ( hex.length() & 1 ) != 0 ) {
		token.append( "0" );
	}

	for ( size_t i = 0; i < hex.length(); ++i ) {
		token += hex[i];

		if ( token.length() == 2 ) {
			out.push_back( std::stoul( token, 0, 16 ) );
			token.clear();
		}
	}
}

void Digest::ToHex( std::string & out, uint8_t * buffer, size_t bufferLen )
{
	static char hex[] = "0123456789abcdef";

	char token[] = "00";

	for ( size_t i = 0; i < bufferLen; i++ ) {
		token[0] = hex[( buffer[i] >> 4 ) & 0x0f];
		token[1] = hex[buffer[i] & 0x0f];
		out.append( token );
	}
}

void Digest::Calculate( std::string & out,
	const std::string & keyId,
	const std::string & keySecretHex,
	uint64_t ts )
{
	std::stringstream ss;
	ss << "key=" << keyId << ";"
	   << "time=" << ts;

	std::string msg = ss.str();

	std::vector<uint8_t> keySecret;
	FromHex( keySecret, keySecretHex );

	uint8_t buffer[EVP_MAX_MD_SIZE];
	unsigned bufferLen;
	HMAC( EVP_sha256(),
		keySecret.data(),
		keySecret.size(),
		reinterpret_cast<const unsigned char *>( msg.c_str() ),
		msg.length(),
		buffer,
		&bufferLen );

	ToHex( out, buffer, bufferLen );
}
