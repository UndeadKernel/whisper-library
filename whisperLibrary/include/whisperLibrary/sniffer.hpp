#ifndef SNIFFER
#define SNIFFER

#include "common.hpp"
#include <vector>
#include <boost/circular_buffer.hpp>
#include <pcap.h>

#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#define IE_TEMPLATE
#else
	/*	Set the following libs as dependency in the linker
		Ws2_32.lib", "wpcap.lib" */
	#include <winsock2.h>
	#include <ws2tcpip.h>

	#ifdef WHISPER_BUILD
		// dll export (Library)
		#define IE_TEMPLATE
	#else
		// dll import (Executable)
		#define IE_TEMPLATE extern
	#	endif
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

IE_TEMPLATE template class WHISPERAPI std::vector<bool>;
IE_TEMPLATE template class WHISPERAPI std::vector<int>;
IE_TEMPLATE template class WHISPERAPI std::vector<char*>;
IE_TEMPLATE template class WHISPERAPI std::vector< std::vector<char*> >;
IE_TEMPLATE template class WHISPERAPI std::vector<pcap_t*>;

namespace whisper_library {
		
	class WHISPERAPI Sniffer {

		public:
			// Default Constructor
			Sniffer();

			// Destructor
			~Sniffer();

			typedef struct {
				struct pcap_pkthdr	header;
				const u_char*		payload;
			} PcapPacket;

			#define RETURN_VALUE(return_code, return_value) \
				m_last_return_codes.push_back(return_code); \
				return (return_value);
			#define RETURN_CODE(return_code) RETURN_VALUE(return_code, return_code)

			// Constants (for m_adapter_data access)
			// unsigned int instead of enum for type safety
			static const unsigned int			ADAPTER_NAME			= 0;
			static const unsigned int			ADAPTER_DESCRIPTION		= 1;
			static const unsigned int			ADAPTER_ADDRESS			= 2;
			static const int					DEFAULT_MAXPACKETSIZE	= 65535;
			static const int					PROMISCUOUS_MODE_ON		= 1;
			static const int					PROMISCUOUS_MODE_OFF	= 0;
			static const int					RETURN_CODE_BUFFER_SIZE = 20;

			enum RC{
				NORMAL_EXECUTION			= 0,
				EMPTY_PACKET_DATA			= 1,
				CLOSE_ON_UNOPENED_HANDLE	= 2,
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
				\fn int adapterCount()
				\brief retrieve the amount of available network adapters with a valid network address
			*/
			int									adapterCount		();
			/**
				\fn std::vector<char*> adapterNames()
				\brief retrieve all adapter/device names (e.g. /dev/eth0) from network adapters with a valid network address
			*/
			std::vector<char*>					adapterNames		();
			/**
				\fn char* adapterName(int adapter_id)
				\brief Get the name from a specific network adapter
			*/
			const char*							adapterName			(int adapter_id);
			/**
			\fn std::vector<char*> adapterAddresses(int adapter_id)
			\brief Get all network addresses from a specific network adapter
			*/
			std::vector<char*>					adapterAddresses	(int adapter_id);
			/**
			\fn int adapterId(char* adapter_value, unsigned int value_type)
			\brief Get the id from an adapter with a specific value\n \
			Value Types:\n ADAPTER_NAME, ADAPTER_DESCRIPTION, ADAPTER_ADDRESS
			*/
			int									adapterId			(const char* adapter_value, unsigned int value_type);
			/**
			\fn boost::circular_buffer<int>* returnCodeBuffer()
			\brief Returns a pointer to the global return code buffer
			*/
			std::vector<int>					lastReturnCodes();
			

			/**
				\fn int retrieveAdapters()
				\brief Retrieves available network devices from the (local) machine
				\return
					0	- normal execution,
					-1	- Error occured,
					-2	- Out of memory
					255 - no adapters found
			*/
			int									retrieveAdapters	();
			/**
				\fn int openAdapter(char* adapterName, int maxPacketSize, int promiscuous)
				\brief Opens a live capture handle to the given device
				\param char*	adapterName		- Name of the adapter (pcap_if_t->name) to open
				\param int		maxPacketSize	- Maximum number of bytes that should be captured from each packet. 
				65535 is enough for the whole packet in most networks.
				\param int		promiscuous 	- Open in promiscuous mode? 0: No, 1: Yes (PCAP_OPENFLAG_PROMISCUOUS).
				promiscuous mode: capture all packets
				non-promiscuous:  capture only packets directed to the application
				\return 0 - normal execution,
				-1 - Error occured
			*/
			int									openAdapter		(const char* adapter_name, int max_packet_size, int promiscuous_mode);
			int									openAdapter		(int adapter_id, int max_packet_size, int promiscuous_mode);
			/**
				\fn int closeAdapter(char* adapter_name)
				\brief Closes an openend handle on the adapter with the given name/id. 
			
			*/
			int									closeAdapter(const char* adapter_name);
			int									closeAdapter(int adapter_name);
			/**
				\fn int freeAdapters()
				\brief Frees unopened adapters and closes previously opened handles.
				\return 0 - normal execution,
				-1 Error occured
			*/
			int									freeAdapters	();
			/**
				\fn int applyFilter(int adapter_id, char* filter)
				\brief Applies a given filter with pcap syntax to the selected adapter
			*/
			int									applyFilter		(int adapter_id, const char* filter);
			int									applyFilter		(const char* adapter_name, const char* filter);
			/**
				\fn int removeFilter(int adapter_id)
				\brief Removes any previously applied filter from the adapter handle
			*/
			int									removeFilter	(int adapter_id);
			int									removeFilter	(const char* adapter_name);

			/**
			\fn const u_char* retrievePacket(int adapter_id)
			\brief Retrieves the next packet from the capture device
			\return PcapPacket{NULL, NULL} if adapter was not found or if the specified adapter had no open handle \n
			or PcapPacket{pcap_pkthdr, NULL} if no packet passed through the configured filter\n
			or PcapPacket{pcap_pkthdr, NULL} if no packet arrived in a system dependent time window (timeout)\n
			or PcapPacket{pcap_pkthdr, const u_char*} where  const u_char* is the pointer to the packet data with the maximum size configured in openAdapter()
			*/
			PcapPacket							retrievePacket(int adapter_id);
			PcapPacket							retrievePacket(const char* adapter_name);

			/**
			\fn std::vector<bool> retrievePacketAsVector(int adapter_id)
			\brief Calls retrievePacket(adapter_id) and converts the retrieved packet payload in a std::vector<bool>
			\return bitwise representation of the packet payload from retrievePacket() as a std::vector<bool> 
			*/
			std::vector<bool>					retrievePacketAsVector	(int adapter_id);
			std::vector<bool>					retrievePacketAsVector	(const char* adapter_name);


	protected:
		/**
		\fn char* ipToString(sockaddr* sockaddr, char* buffer, size_t buffer_size)
		\brief Converts a given socket address to a human readable string
		*/
		char* ipToString(sockaddr* sockaddr, char* buffer, size_t buffer_size);

	private:
		/* adapters_data[x][y]
		x : adapter number
		[x][ADAPTER_NAME] : adapter/device name (e.g. /dev/eth0)
		[x][ADAPTER_DESCRIPTION] : adapter description
		[x][ADAPTER_ADDRESS+] : adapter addresses (NULL if not available)
		*/
		std::vector< std::vector<char*> >	m_adapter_data;
		std::vector<pcap_t*>				m_adapter_handles;

		pcap_if_t*							m_adapter_raw_data;
		// Stores the last 20 method return codes
		boost::circular_buffer<int>			m_last_return_codes;
		bool								checkForAdapterId(int adapter_id);
		int									adapterId(const char* value, unsigned int key, bool increment_key);
	};
}
#endif // SNIFFER