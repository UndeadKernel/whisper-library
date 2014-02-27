//author: Simon Kadel

#ifndef RAW_SOCKET_PROTOCOL
#define RAW_SOCKET_PROTOCOL

#include <boost/asio.hpp>

namespace boost {
namespace asio {
namespace ip {

template <int P> class RawSocketProtocol {
	public:
		//default constructor

		explicit RawSocketProtocol(): m_protocol(P), m_family(PF_INET) {
		}

		// The RAW socket type. TODO: needed?
		typedef boost::asio::basic_raw_socket<RawSocketProtocol> socket;

		// The RAW resolver type.
		typedef boost::asio::ip::basic_resolver<RawSocketProtocol> resolver;
		
		// The type of a RAW endpoint.
		typedef boost::asio::ip::basic_endpoint<RawSocketProtocol> endpoint;	

		// Construct to represent the IPv4 RAW protocol.
		static RawSocketProtocol v4(){
			return RawSocketProtocol(P, PF_INET);
		}

		// Construct to represent the IPv6 RAW protocol.
		static RawSocketProtocol v6(){
			return RawSocketProtocol(P, PF_INET6);
		}

		 // Compare two protocols for equality.
		friend bool operator==(const RawSocketProtocol& p1, const RawSocketProtocol& p2){
			 return p1.m_protocol == p2.m_protocol && p1.m_family == p2.m_family;
		}

		// Compare two protocols for inequality.
		friend bool operator!=(const RawSocketProtocol& p1, const RawSocketProtocol& p2){
			return p1.m_protocol != p2.m_protocol || p1.m_family != p2.m_family;
		 }

		// protocol type identifier
		int type() const {return SOCK_RAW;}	

		//-- member variable getters
		int protocol() const {return m_protocol;}
		int family() const {return m_family;}


	private:
		explicit RawSocketProtocol(int protocol, int family)
			:m_protocol(protocol), m_family(family) {
		}

		int m_protocol; //protocol identifier
		int m_family;	//protocol family indentifier
	};
}
}
}
#endif //RAW_SOCKET_PROTOCOL