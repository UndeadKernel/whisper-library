/*	<pcapwrapper.hpp>
Copyright(C) 2013,2014  Jan Simon Bunten
Simon Kadel
Martin Sven Oehler
Arne Sven Stühlmeyer

This File is part of the WhisperLibrary

WhisperLibrary is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WhisperLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PCAPWRAPPER
#define PCAPWRAPPER

#include <vector>
#include <boost/circular_buffer.hpp>
#include <pcap.h>
#include <string>

#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
#else
	/*	Set the following libs as dependency in the linker
		Ws2_32.lib", "wpcap.lib" */
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif

/* Define PCAP_NETMASK_UNKNOWN if not included on pcap.h */
#ifndef PCAP_NETMASK_UNKNOWN
	#define PCAP_NETMASK_UNKNOWN    0xffffffff
#endif

#ifdef DEBUG_LEVEL
/* define debug macro
the do {} while(0) ensures that the ; after DEBUG() is always correctly placed.
disabled compiler warning: C4127 (conditional expression is constant) */
#ifdef WIN32
#define DEBUG(MinLevel, message, ...) __pragma( warning( push ) ) \
	__pragma(warning(disable : 4127)) \
do { \
if (DEBUG_LEVEL >= MinLevel) { fprintf(stdout, message, ##__VA_ARGS__); } \
} while (0) __pragma(warning(pop))
#else
#define DEBUG(MinLevel, message, ...)  \
do { if (DEBUG_LEVEL >= MinLevel) { \
	fprintf(stdout, message, ##__VA_ARGS__); } } while (0)
#endif
#else
#define DEBUG(...)
#endif

#ifndef PCAP_NETMASK_UNKNOWN 
#define PCAP_NETMASK_UNKNOWN    0xffffffff 
#endif

namespace whisper_library {
		
	class PcapWrapper {

	public:
		// Default Constructor
		PcapWrapper();

		// Destructor
		~PcapWrapper();

		/** 
			\brief Packet structure as returned by pcap
		*/
		typedef struct {
			struct pcap_pkthdr	header; ///< pcap header containing packet arrival timestamps
			const u_char*		payload; ///< packet data
		} PcapPacket;

		#define RETURN_VALUE(return_code, return_value) \
			m_last_return_codes.push_back(return_code); \
			return (return_value);
		#define RETURN_CODE(return_code) RETURN_VALUE(return_code, return_code)

		/* Constants (for m_adapter_data access)
		unsigned int instead of enum for type safety */
		static const int ADAPTER_NAME = 0; ///< value type ID for the adapter name (used as parameter in adapterId(...))
		static const int ADAPTER_DESCRIPTION = 1; ///< value type ID for the adapter description (used as parameter in adapterId(...))
		static const int ADAPTER_ADDRESS = 2; ///< value type ID for the adapter address (used as parameter in adapterId(...))
		static const int DEFAULT_MAXPACKETSIZE = 65535; ///< default maximal packet size to capture entire packets (used as parameter in openAdapter(...))
		static const int RETURN_CODE_BUFFER_SIZE = 20; ///< defines the size of the last return codes-buffer (retrieved via lastReturnCodes()).
		static const int TIMEOUT_WAIT_FOREVER = 0; ///< timeout value for openAdapter which can cause retrievePacket(...) and retrievePacketAsVector(...) to wait forever til packets arrive

		/**
			\brief Possible return codes issued by PcapWrapper Methods
		*/
		enum RC{
			NORMAL_EXECUTION			= 0,
			EMPTY_PACKET_DATA			= 1,
			CLOSE_ON_UNOPENED_HANDLE	= 2,
			OPEN_ON_OPENED_HANDLE		= 3,
			NO_ADAPTERS_FOUND			= 255,
			UNSPECIFIED_ERROR_OCCURED	= -1,
			ADAPTER_NOT_FOUND			= -2,
			ACCESS_ON_UNOPENED_HANDLE	= -3,
			ERROR_RETRIEVING_ADAPTERS	= -4,
			ERROR_OPENING_HANDLE		= -5,
			ERROR_COMPILING_FILTER		= -6,
			ERROR_APPLYING_FILTER		= -7,
			OUT_OF_MEMORY				= -254,
		};

		// Getter & Setter

		/**
			\brief retrieve the amount of available network adapters with a valid network address
		*/
		int adapterCount();
		/**
			\brief retrieve all adapter/device names (e.g. /dev/eth0) from network adapters with a valid network address
		*/
		std::vector<std::string> adapterNames(); 
		/**
			\brief Get the name from a specific network adapter
		*/
		std::string adapterName(int adapter_id);
		/**
		\brief Get all network addresses from a specific network adapter
		*/
		std::vector<std::string> adapterAddresses(int adapter_id);

		/**
			\brief Get the descriptional text from a specific network adapter (under windows e.g. the name of the hardware network device, under unix often "<null>")
		*/
		std::string adapterDescription(int adapter_id);
		/**
		\brief Get the id from an adapter with a specific value\n \
		Value Types:\n ADAPTER_NAME, ADAPTER_DESCRIPTION, ADAPTER_ADDRESS
		*/
		int adapterId(std::string adapter_value, int value_type);
		/**
		\brief Returns a pointer to the global return code buffer
		*/
		std::vector<int> lastReturnCodes();

		/**
			\brief empties the PcapWrapper's global return code buffer
		*/
		void clearReturnCodes();
			
		/**
			\brief Retrieves available network devices from the (local) machine
			\return
				0	- normal execution,
				-1	- Error occured,
				-2	- Out of memory
				255 - no adapters found
		*/
		int retrieveAdapters();
		/**
			\brief Opens a live capture handle to the given device
			\param adapter_name		- Name of the adapter (pcap_if_t->name) to open
			\param max_packet_size	- Maximum number of bytes that should be captured from each packet. 
			65535 is enough for the whole packet in most networks.
			\param promiscuous_mode - Open in promiscuous mode? false: No, true: Yes.
			\param timeout			- Specifies a timeout in ms. A value of 0 can cause the process to wait forever.
			promiscuous mode: capture all packets
			non-promiscuous:  capture only packets directed to the application
			\return 0 - normal execution,
			-1 - Error occured
		*/
		int openAdapter(std::string adapter_name, int max_packet_size, bool promiscuous_mode, int timeout);
		/**
			\brief Opens a live capture handle to the given device
			\param adapter_id		- internal id of the adapter to open
			\param max_packet_size		- Maximum number of bytes that should be captured from each packet.
			65535 is enough for the whole packet in most networks.
			\param promiscuous_mode 	- Open in promiscuous mode? false: No, true: Yes.
			\param timeout				- Specifies a timeout in ms. A value of 0 can cause the process to wait forever.
			promiscuous mode: capture all packets
			non-promiscuous:  capture only packets directed to the application
			\return 0 - normal execution,
		-1 - Error occured
		*/
		int	openAdapter(int adapter_id, int max_packet_size, bool promiscuous_mode, int timeout);
		/**
			\brief Closes an openend handle on the adapter with the given name/id. 
		*/
		int closeAdapter(std::string adapter_name);
		/**
			\brief Closes an openend handle on the adapter with the given name/id.
		*/
		int	closeAdapter(int adapter_id);
		/**
			\brief Frees unopened adapters and closes previously opened handles.
			\return 0 - normal execution,
			-1 Error occured
		*/
		int	freeAdapters();
		/**
			\brief Applies a given filter with pcap syntax to the selected adapter
		*/
		int	applyFilter(int adapter_id, std::string filter);
		/**
			\brief Applies a given filter with pcap syntax to the selected adapter
		*/
		int	applyFilter(std::string adapter_name, std::string filter);
		/**
			\brief Removes any previously applied filter from the adapter handle
		*/
		int	removeFilter(int adapter_id);
		/**
			\brief Removes any previously applied filter from the adapter handle
		*/
		int	removeFilter(std::string adapter_name);

		/**
			\brief Retrieves the next packet from the capture device
			\return PcapPacket{NULL, NULL} if adapter was not found or if the specified adapter had no open handle \n
			or PcapPacket{pcap_pkthdr, NULL} if no packet passed through the configured filter\n
			or PcapPacket{pcap_pkthdr, NULL} if no packet arrived in a system dependent time window (timeout)\n
			or PcapPacket{pcap_pkthdr, const u_char*} where  const u_char* is the pointer to the packet data with the maximum size configured in openAdapter()
		*/
		PcapPacket retrievePacket(int adapter_id);
		/**
			\brief Retrieves the next packet from the capture device
			\return PcapPacket{NULL, NULL} if adapter was not found or if the specified adapter had no open handle \n
			or PcapPacket{pcap_pkthdr, NULL} if no packet passed through the configured filter\n
			or PcapPacket{pcap_pkthdr, NULL} if no packet arrived in a system dependent time window (timeout)\n
			or PcapPacket{pcap_pkthdr, const u_char*} where  const u_char* is the pointer to the packet data with the maximum size configured in openAdapter()
		*/
		PcapPacket retrievePacket(std::string adapter_name);

		/**
			\brief Calls retrievePacket(adapter_id) and converts the retrieved packet payload in a std::vector<bool>
			\return bitwise representation of the packet payload from retrievePacket() as a std::vector<bool> 
		*/
		std::vector<bool> retrievePacketAsVector(int adapter_id);
		/**
			\brief Calls retrievePacket(adapter_id) and converts the retrieved packet payload in a std::vector<bool>
			\return bitwise representation of the packet payload from retrievePacket() as a std::vector<bool>
		*/
		std::vector<bool> retrievePacketAsVector(std::string adapter_name);

		/**
		\brief Sends a packet using a raw socket via the WinPcap driver (Note: libcap on linux currently doesn't allow sending packages)
		\return RC.NORMAL_EXECUTION - normal execution
				RC.ADAPTER_NOT_FOUND - adapter with given id not found
				RC.ACCESS_ON_UNOPENED_HANDLE - tried to access an unopened adapter (use openAdapter(...) beforehand)
				RC.UNSPECIFIED_ERROR_OCCURED - otherwise
		*/
		int sendPacket(int adapter_id, unsigned char* packet_buffer, int buffer_size);
		/**
			\brief Sends a packet using a raw socket via the WinPcap driver (Note: libcap on linux currently doesn't allow sending packages)
			\return RC.NORMAL_EXECUTION - normal execution
			RC.ADAPTER_NOT_FOUND - adapter with given id not found
			RC.ACCESS_ON_UNOPENED_HANDLE - tried to access an unopened adapter (use openAdapter(...) beforehand)
			RC.UNSPECIFIED_ERROR_OCCURED - otherwise
		*/
		int	sendPacket(std::string adapter_name, unsigned char* packet_buffer, int buffer_size);
		/**
			\brief Sends a packet using a raw socket via the WinPcap driver (Note: libcap on linux currently doesn't allow sending packages)
			\return RC.NORMAL_EXECUTION - normal execution
			RC.ADAPTER_NOT_FOUND - adapter with given id not found
			RC.ACCESS_ON_UNOPENED_HANDLE - tried to access an unopened adapter (use openAdapter(...) beforehand)
			RC.UNSPECIFIED_ERROR_OCCURED - otherwise
		*/
		int	sendPacket(int adapter_id, std::vector<bool> packet_data);
		/**
			\brief Sends a packet using a raw socket via the WinPcap driver (Note: libcap on linux currently doesn't allow sending packages)
			\return RC.NORMAL_EXECUTION - normal execution
			RC.ADAPTER_NOT_FOUND - adapter with given id not found
			RC.ACCESS_ON_UNOPENED_HANDLE - tried to access an unopened adapter (use openAdapter(...) beforehand)
			RC.UNSPECIFIED_ERROR_OCCURED - otherwise
		*/
		int	sendPacket(std::string adapter_name, std::vector<bool> packet_data);

	private:
		/*
		adapters_data[x][y]
		x : adapter number
		[x][ADAPTER_NAME] : adapter/device name (e.g. /dev/eth0)
		[x][ADAPTER_DESCRIPTION] : adapter description
		[x][ADAPTER_ADDRESS+] : adapter addresses (NULL if not available)
		*/
		std::vector< std::vector<std::string> >	m_adapter_data; ///< Stores for each adapter its name, the description and each address

		std::vector<pcap_t*> m_adapter_handles; ///< Contains all open Adapter handles for each adapter, NULL otherwise
		std::vector<bpf_u_int32> m_adapter_netmasks; ///< Contains the netmask for each adapter
		pcap_if_t* m_adapter_raw_data; ///< Raw adapter data as returned from pcap_findalldevs(...)

		boost::circular_buffer<int>	m_last_return_codes; ///< Stores the last RETURN_CODE_BUFFER_SIZE method return codes

		/** checkForAdapterId
			\brief Checks if a given adapter_id exists.
		*/
		bool checkForAdapterId(int adapter_id);
		/**
			\brief Converts a given socket address to a human readable string
		*/
		char* ipToString(sockaddr* sockaddr, char* buffer, size_t buffer_size);

		/* Private Getter & Setter */

		/**
			\brief returns the adapter id from the adapter with the given key.
		*/
		int	adapterId(std::string value, int key, bool increment_key);
	};
}
#endif // PCAPWRAPPER