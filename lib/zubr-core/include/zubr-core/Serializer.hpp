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

/// Serializer.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __ZUBR_SERIALIZER__H
#define __ZUBR_SERIALIZER__H


#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace zubr {

	class Serializer;

	class Serializable {
	public:
		virtual void Serialize( Serializer & s )
		{
		}

		virtual void Deserialize( Serializer & s )
		{
		}
	};

	class Serializer {
	public:
		virtual std::shared_ptr<Serializer> AddObject(
			const std::string & memberName = "" )
			= 0;

		virtual std::shared_ptr<Serializer> GetObject(
			const std::string & memberName = "" )
			= 0;

		virtual Serializer & Serialize(
			int64_t v, const std::string & memberName = "" )
			= 0;

		virtual Serializer & Serialize(
			const std::string & v, const std::string & memberName = "" )
			= 0;

		virtual Serializer & Serialize(
			Serializable & v, const std::string & memberName = "" )
			= 0;

		virtual Serializer & Deserialize( int & out,
			const std::string & memberName = "",
			int defaultValue = -1 )
			= 0;

		virtual Serializer & Deserialize( int64_t & out,
			const std::string & memberName = "",
			int64_t defaultValue = INT64_C( -1 ) )
			= 0;

		virtual Serializer & Deserialize( std::string & out,
			const std::string & memberName = "",
			const std::string & defaultValue = "" )
			= 0;

		virtual Serializer & Deserialize(
			Serializable & v, const std::string & memberName = "" )
			= 0;

		virtual Serializer & Deserialize(
			const std::function<void(
				Serializer &, const std::string & memberName )> & add,
			const std::string & memberName = "" )
			= 0;

		template <typename TItem>
		Serializer & Deserialize(
			std::vector<TItem> & v, const std::string & memberName = "" )
		{

			Deserialize(
				[this, &v]( Serializer & s, const std::string & memberName ) {
					TItem item;
					item.Deserialize( s );
					v.push_back( item );
				},
				memberName );

			return *this;
		}

		template <typename TItem>
		Serializer & Deserialize( std::unordered_map<int, TItem> & v )
		{

			Deserialize(
				[this, &v]( Serializer & s, const std::string & memberName ) {
					TItem item;
					item.Deserialize( s );
					v[std::stoi( memberName )] = item;
				} );

			return *this;
		}

		virtual void ToString( std::string & out ) = 0;
		virtual void FromString( const std::string & s ) = 0;
	};

	class SerializerFactory {
	public:
		virtual std::shared_ptr<Serializer> Create() const = 0;
	};

} // namespace zubr


#endif
