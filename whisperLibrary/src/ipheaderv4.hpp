#ifndef IP_PACKET_V4
#define IP_PACKET_V4

#include <vector>
#include <string>

using namespace std;

namespace whisper_library {
/** \brief Represents an Ipv4 header

	This class provides getters and setters for all Ipv4 header fields, except the options-field.
	The content is exported as binary by using 'toVector'.
*/
class IpHeaderv4 {
public:
	/**
		\brief Constructor that initializes all fields with 0.
	*/
	IpHeaderv4();
	/** \brief Constructor that extracts the Ipv4 header from a network frame
		
		\param frame binary network frame in little endian	
	*/
	IpHeaderv4(vector<bool> frame);
	/**
		\brief Destructor
	*/
	~IpHeaderv4();
	/** \brief Calculates the length of the header and sets the corresponding field
	*/
	void calculateHeaderLength();
	/** \brief Calculates the checksum and sets the corresponding field

		Since the options field is not supported yet, the default value '5' is set (5x32bit)
	*/
	void calculateChecksum();
	/** \brief Prints the content of the ip header as hexadecimal
	*/
	void print();
	/** \brief Creates a string which lists all fields and values in humanly readable form
		\return String with all field values
	*/
	string info();
	/** Converts the ip header to binary
		/return binary representation of the ip header in little endian
	*/
	vector<bool> toVector();

	/** \brief Returns the IP-Version (4 or 6) (4 bit)
	*/
	unsigned int version();
	/** \brief Returns the length of the IP header : length * 32 bit (4 bit)
	*/
	unsigned int ipHeaderLength();
	/** \brief Returns the type of service, which is used for packet prioritization (8 bit)
	*/
	unsigned int typeOfService(); 	
	/** \brief Returns the total length of the packet in byte(maximum 65536 Byte = 64 KiB) (16 bit)
	*/
	unsigned int totalLength();
	/** \brief Returns the unique identifier to reassemble fragmented IP-Datagrams (16 bit)
	*/
	unsigned int identification();
	/** \brief Returns the reserved flag, has to be 0 (1 bit)
	*/
	bool flagReserved();
	/** \brief Returns the 'don't fragment' bit
	*/
	bool flagDontFragment();
	/** \brief Returns a flag that indicates, wether there are more fragments coming 
			   or not (0 if only or last fragment) (1 bit)
	*/
	bool flagFragmented(); 	
	/** \brief Returns the fragment position (0 if not fragmented) (12 bit)
	*/
	unsigned int offset();
	/** \brief Returns the time to live, drop packet if 0, reduce at each hop (4 bit)
	*/
	unsigned int timeToLive();
	/** \brief Returns the application layer protocol id (4 bit)
	*/
	unsigned int protocol();
	/** \brief Returns the Ip header checksum (16 bit)
	*/
	unsigned int checksum(); 
	/** \brief Returns the Ip address of the sender (32 bit)
	*/
	unsigned long sourceIp();
	/** \brief Returns the ip address of the sender in dotted form
	*/
	string sourceIpDotted();
	/** \brief Returns the Ip address of the receiver(32 bit)
	*/ 
	unsigned long destinationIp();
	/** \brief Returns the Ip address of the receiver in dotted form
	*/
	string destinationIpDotted();

	/** \brief Sets the IP-Version (4 or 6) (4 bit)
	*/
	void setVersion(unsigned int version);
	/** \brief Sets the type of service, which is used for packet prioritization (8 bit)
	*/
	void setTypeOfService(unsigned int tos);
	/** \brief Sets the total length of the packet in byte (maximum 65536 Byte = 64 KiB) (16 bit)
	*/
	void setTotalLength(unsigned int total_length);
	/** \brief Sets the unique identifier to reassemble fragmented IP-Datagrams (16 bit)
	*/
	void setIdentification(unsigned int identification);
	/** \brief Sets the reserved flag, has to be 0 (1 bit)
	*/
	void setFlagReserved(bool flag);
	/** \brief Sets the 'don't fragment' bit
	*/
	void setFlagdontFragment(bool flag);
	/** \brief Sets a flag that indicates, wether there are more fragments coming
	or not (0 if only or last fragment) (1 bit)
	*/
	void setFlagFragmented(bool flag);
	/** \brief Sets the fragment position (0 if not fragmented) (12 bit)
	*/
	void setOffset(unsigned int offset);
	/** \brief Sets the time to live, drop packet if 0, reduce at each hop (4 bit)
	*/
	void setTimeToLive(unsigned int ttl);
	/** \brief Sets the application layer protocol id (4 bit)
	*/
	void setProtocol(unsigned int protocol_id);
	/** \brief Sets the Ip address of the sender (32 bit)
	*/
	void setSourceIp(unsigned long source_ip);
	/** \brief Sets the ip address of the sender in dotted form
	*/
	void setSourceIp(string source_ip);
	/** \brief Sets the Ip address of the receiver(32 bit)
	*/
	void setDestinationIp(unsigned long destination_ip);
	/** \brief Sets the Ip address of the receiver in dotted form
	*/
	void setDestinationIp(string destination_ip);

	static const unsigned int TCP = 6; ///< Protocol id of tcp
	static const unsigned int UDP = 17; ///< Protocol id of udp

private:
	/** \brief Converts an ip to its dotted form
		\param ip ip as decimal
		\return ip in dotted form
	*/
	string ipToDotted(unsigned long ip);
	/** \brief Converts an ip to decimal
		\param ip ip in dotted form
		\return ip as decimal
	*/
	unsigned long ipToUlong(string ip);
	unsigned char* m_head; ///< Holds the 20 byte ip header
};
}

#endif