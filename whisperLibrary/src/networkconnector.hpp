#ifndef NETWORK_CONNECTOR
#define NETWORK_CONNECTOR

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <thread>
#include <atomic>

#include <genericpacket.hpp>
#include "tcppacket.hpp"
#include "udppacket.hpp"
#include <pcapwrapper.hpp>
#include "socketSender.hpp"
#include <covertchannel.hpp>

#ifdef WIN32
	#include <iphlpapi.h>
#endif

using namespace std;

namespace whisper_library {
	/** \brief NetworkConnector can send Packets to the Network
	
		A NetworkConnector is used by the ChannelManager to send packets. It can be configured
		to use a specific network adapter and listen to packets from specific Ip addresses.
	*/

class NetworkConnector {
public:
	/** \brief Constructor of NetworkConnector
		
		\param packet_received Callback function that is called, when a packet arrived over network. The first
								parameter is the ip it was sent from, the second parameter is the application layer
								part of the packet in a generic format.
	*/
	NetworkConnector(function<void(string, GenericPacket)> packet_received);

	/** \brief Destructor of NetworkConnector
	*/
	~NetworkConnector();

	// Interface CovertChannel
	/** \brief Sends a tcp packet to a specific ip
		
		\param ip The ip the packet is send to.
		\param packet The tcp packet that is send over network.
	
	*/
	void sendTcp(string ip, TcpPacket packet);
	/** \brief Sends a udp packet to a specific ip

	\param ip The ip the packet is send to.
	\param packet The udp packet that is send over network.

	*/
	void sendUdp(string ip, UdpPacket packet);

	// Interface ChannelManager
	/** \brief Opens a network listener for packets of a specific ip and covert channel.

		Opens a network adapter that has to be set via 'setAdapter' and listens for packets
		that are received from the given ip, port and protocol as specified in the covert channel.
		The listener is started in a seperate thread.
		Received packets are returned with the callback method set in the constructor.
		\param ip The destination IP of the connection
		\param channel The covert channel that is used for the connection
		\return Returns true if the packet listener was started succesfully, otherwise false.
	*/
	bool openListener(string ip, CovertChannel* channel);
	/** \brief Closes the network listener for a specific ip
	
		Stops listening for packets coming from the given ip. If no open connection remains,
		the network adapter is closed. 
	*/
	void closeListener(string ip);

	/** \brief Returns a vector with available network adapters.
		
		Returns a vector with all available network adapter names. The name uniquely identifies
		the network adapter and is used to select it with 'setAdapter'.

		\return Vector with all available network adapter names.
	*/
	vector<string> adapters();
	/** \brief Returns the number of avaiable network adapters.
	*/
	unsigned int adapterCount();
	/** \brief Sets the adapter that is used to listen for packets and send them (sending win32 only).
		
		Sets the network adapter that is used to listen for incoming packets. Has no effect if an adapter
		is open. Sending tcp using that adapter is only available on win32 using wpcap. 
		The unique names of the adapters can be retrieved by calling 'adapters'.

		\param adapter_name Unique name of the adapter
	*/
	void setAdapter(string adapter_name);
	/** \brief Returns the description of a given network adapter.
		
		Returns the description of a given network adapter. 
		The unique names of the adapters can be retrieved by calling 'adapters'.
		\param adapter_name Unique name of the adapter
		\return Description of the adapter. Is empty ("") if the adapter_name wasn't found or the description is NULL.
	*/
	string adapterDescription(string adapter_name);
	/** \brief Returns a vector with all network addesses of the currently selected adapter.
		\return Vector of network addresses of the currently selected adapter
	*/
	vector<string> adapterAddresses();

private:
	/** \brief Loop that retrieves packets from the selected network adapter.

		Starts a loop that runs as long as an adapter is open. It retrieves packets from the
		selected adapter and forwards the application layer part via the callback function m_packet_received,
		that is set in the constructor. Packets are retrieved with a maximum delay of 1 ms.
	*/
	void retrievePacket();
	/** \brief Checks if the given ip is a valid IPv4 address.
		\param ip The ip to check
		\return True if the ip is a valid Ipv4 address, otherwise false
	*/
	bool validIPv4(string ip);
	/** \brief Adds a capture filter for the opened adapter.
		
		Only packets with matching ip, port and protocol are received.
		Multiple filters are linked with 'or'.
		/param ip The ip, that packets are received from in dotted form
		/param port the port, that received packets should have
		/param protocol the protocol, that received packets should have
	*/
	void addFilter(string ip, unsigned short port, string protocol);
	/** \brief Removes a capture filter
		\param ip the ip, that you no longer want to receive messages from
	*/
	void removeFilter(string ip);
	/** \brief Has to be called when the filter map 'm_filter' has changed to refresh the capture filter.
	*/
	void rebuildFilter();
	/** \brief Switches binary from big endian to little endian and the other way around
		\return Vector that contains the switched binary
	*/
	vector<bool> switchEndian(vector<bool> binary);
	/** \brief Is called in a new thread when a packet is received.
		
		The function extracts the application layer header and data from 
		the complete frame and transforms it to little endian. It then calls the callback function m_packet_received
		to give the packet back to the channelmanager.

		\param packet_data binary representation of a complete frame, that was retrieved from an adapter
	
	*/
	void packetReceived(vector<bool> packet_data);

	PcapWrapper* m_pcap; ///< Pointer to the pcap wrapper. Is used to listen for packets and sending packets.
	SocketSender* m_socket; ///< Pointer to a socket class. Is used to send packets using a raw socket.

	// Win32 only
#ifdef WIN32
	typedef struct {
		unsigned char		mac_address[6]; ///< 6 byte mac address
		IPAddr				gateway_address; ///< default gateway address
	} AdapterInfo;

	/**
		\fn win32GetAdapterInfo()
		\brief Retrieves the MAC-Address of the current m_adapter and its first (default) gateway (if set)
		\return a struct of type AdapterInfo, containing the MAC address and the default gateway address.
	*/
	AdapterInfo win32GetAdapterInfo();
	/** \brief Returns the mac address of a destination ip

		Checks the cache for an entry for the given destination ip. If no entry was found, it sends an arp request for that ip.
		If there is no answer, it returns the mac of the default gateway.
		/param source_ip your local ipv4 address
		/param destination_ip ipv4 address of the destination host whose MAC you want to retrieve
		/param adapter_info adapterInfo struct of the currently used adapter. Is retrieved by calling 'win32GetAdapterInfo'.
		\return Returns the mac address as a hex string in the format ff:ff:ff:ff:ff:ff
	*/
	string win32GetDestinationMAC(string source_ip, string destination_ip, AdapterInfo adapter_info);
	/** \brief Uses ARP to get the mac address of a destination ip

		\param source_ip own ip
		\param destination_ip ip of the destination host whose MAC you want to retrieve
		\return Returns the mac address as a hex string in the format ff:ff:ff:ff:ff:ff

	*/
	string win32SendArp(IPAddr source_ip, IPAddr destination_ip);

	PIP_ADAPTER_ADDRESSES m_adapter_addresses; ///< List of IP_ADAPTER_ADDRESSES from getAdapterAddresses() (Win32-only)
	map<string,string> m_destination_macs;	   ///< arp request cache that stores mac addresses of IPs
	PcapWrapper* m_pcap_sender;	///< instance of PcapWrapper, that is only used to send tcp packets.
#endif

	string m_source_ip;	///< your own ip (ipv4), is set after changing the adapter (setAdapter)
	unsigned int m_connection_count; ///< Counts the number of open connections
	string m_adapter; ///< Holds the unique adapter name, that is currently selected
	atomic<bool> m_adapter_open; ///< True, if a network adapter is open. Otherwise false.
	map<string, string> m_filter; ///< map, that stores the capture filter rule for every ip. The key is the ip, the capture filter belongs to.
	function<void(string, GenericPacket)> m_packet_received; /**< Callback method that is called, when a new packet arrived. 
															 First parameter is the ip it was sent from, the second is the application layer 
															 part of the packet in a generic format */
};

}

#endif // NETWORK_CONNECTOR