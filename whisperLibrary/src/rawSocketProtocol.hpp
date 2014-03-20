/*	<socketSender.hpp>
	Copyright(C) 2013, 2014	Jan Simon Bunten
							Simon Kadel
							Martin Sven Oehler
							Arne Sven Stühlmeyer
	
	Based on raw.hpp (c) 2012 Kevin D. Conley (kcon at stanford dot edu)

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RAW_SOCKET_PROTOCOL
#define RAW_SOCKET_PROTOCOL

#include <boost/asio.hpp>

namespace boost {
namespace asio {
namespace ip {

/** \brief Protocol for using raw sockets
	
*/
template <int P> class RawSocketProtocol {

public:
	/** \brief default constructor
	*/
	explicit RawSocketProtocol(): m_protocol(P), m_family(PF_INET) {}
	/** \brief The RAW socket type.
	*/ 
	typedef boost::asio::basic_raw_socket<RawSocketProtocol> socket;
	/** \brief The RAW resolver type.
	*/ 
	typedef boost::asio::ip::basic_resolver<RawSocketProtocol> resolver;
	/** \brief The type of a RAW endpoint.
	*/ 
	typedef boost::asio::ip::basic_endpoint<RawSocketProtocol> endpoint;	
	/** \brief Construct to represent the IPv4 RAW protocol.
	*/ 
	static RawSocketProtocol v4(){
		return RawSocketProtocol(P, PF_INET);
	}
	/** \brief Construct to represent the IPv6 RAW protocol.
	*/ 
	static RawSocketProtocol v6(){
		return RawSocketProtocol(P, PF_INET6);
	}
	/** \brief Compare two protocols for equality.
		*/ 
	friend bool operator==(const RawSocketProtocol& p1, const RawSocketProtocol& p2){
			return p1.m_protocol == p2.m_protocol && p1.m_family == p2.m_family;
	}
	/** \brief Compare two protocols for inequality.
	*/ 
	friend bool operator!=(const RawSocketProtocol& p1, const RawSocketProtocol& p2){
		return p1.m_protocol != p2.m_protocol || p1.m_family != p2.m_family;
		} 
	/** \brief protocol type identifier
	*/
	int type() const {
		return SOCK_RAW;
	}	
	/** \return the protocol identifier
	*/
	int protocol() const {
		return m_protocol;
	}
	/** \return the protocol family indentifier
	*/
	int family() const {
		return m_family;
	}

private:
	/** \brief private constructor
	*/
	explicit RawSocketProtocol(int protocol, int family)
		:m_protocol(protocol), m_family(family) {
	}

	int m_protocol; ///<protocol identifier
	int m_family;///<protocol family indentifier
};
}
}
}
#endif //RAW_SOCKET_PROTOCOL