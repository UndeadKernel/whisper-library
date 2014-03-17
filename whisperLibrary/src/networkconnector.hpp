#ifndef NETWORK_CONNECTOR
#define NETWORK_CONNECTOR

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <thread>

#include <genericpacket.hpp>
#include "tcppacket.hpp"
#include "udppacket.hpp"
#include <pcapwrapper.hpp>
#include "socketSender.hpp"
#include <covertchannel.hpp>

using namespace std;

namespace whisper_library {

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
		Received packets are returned with the callback method set in the constructor.
		\param ip The destination IP of the connection
		\param The covert channel that is used for the connection
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
		
		Sets the network adapter that is used to listen for incoming packets. Sending tcp using that adapter is
		only available on win32 using wpcap. The unique names of the adapters can be retrieved by calling 'adapters'.

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
		\returns Vector of network addresses of the currently selected adapter
	*/
	vector<string> adapterAddresses();

private:
	void retrievePacket();
	bool validIP(string ip);
	void addFilter(string ip, unsigned short port, string protocol);
	void removeFilter(string ip);
	void rebuildFilter();
	vector<bool> switchEndian(vector<bool> big_endian);
	PcapWrapper* m_pcap;
	SocketSender* m_socket;



	unsigned int m_connection_count;
	string m_adapter;
	bool m_adapter_open;
	map<string, string> m_filter;
	function<void(string, GenericPacket)> m_packet_received;
};

}

#endif // NETWORK_CONNECTOR