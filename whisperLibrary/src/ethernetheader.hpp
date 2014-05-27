#ifndef ETHERNET_HEADER
#define ETHERNET_HEADER

#include <string>
#include <vector>

using namespace std;
namespace whisper_library {
/** \brief Represents an ethernet header

This class provides getters and setters for all ethernet header fields (IEEE 802.3 without extensions)
The content is exported as binary by using 'toVector'.
*/
class EthernetHeader {
public:
	/** \brief Constructor that initializes the header with 0
	*/
	EthernetHeader();
	/** \brief Constructor that initializees the header with the given char array
		\param header pointer to char array of 14 byte (char <-> 1 byte)
	*/
	EthernetHeader(unsigned char* header);
	/** \brief Destructor
	*/
	~EthernetHeader();
	/** \brief Returns the source mac address in the format ff:ff:ff:ff:ff:ff (hexadecimal, bytes seperated by :)
	*/
	string sourceMAC();
	/** \brief Returns the destination mac address in the format ff:ff:ff:ff:ff:ff (hexadecimal, bytes seperated by :)
	*/
	string destinationMAC();
	/** \brief Returns the ethernet type (2048<-> ipv4)
	*/
	unsigned long ethernetType();

	/** \brief Sets the source mac address in the format ff:ff:ff:ff:ff:ff (hexadecimal, bytes seperated by :)
	*/
	void setSourceMAC(string mac);
	/** \brief Sets the source mac address
		\param char_array pointer to a char array of 6 bytes
	*/
	void setSourceMAC(unsigned char* char_array);
	/** \brief Sets the destination mac address in the format ff:ff:ff:ff:ff:ff (hexadecimal, bytes seperated by :)
	*/
	void setDestinationMAC(string mac);
	/** \brief Sets the destination mac address
		\param char_array pointer to a char array of 6 bytes
	*/
	void setDestinationMAC(unsigned char* char_array);
	/** \brief Sets the ethernet type (2048<-> ipv4)
	*/
	void setEthernetType(unsigned long type);
	/** \brief Converts a binary mac address to a string in the format ff:ff:ff:ff:ff:ff (hexadecimal, bytes seperated by :)
		\param buffer pointer to a char array of 6 byte
		\return mac address as string
	*/
	static string toMacString(unsigned char* buffer);
	/** \brief Returns the content of the header as binary in little endian
	*/
	vector<bool> toVector();
	/** \brief Creates a string which lists all fields and values in humanly readable form
		\return String with all field values
	*/
	string toString();
private:
	/** \brief Sets either the source or destination mac address depending on the data_offset
	\param mac address in the format: ff:ff:ff:ff:ff:ff (hexadecimal, bytes seperated by :)
	\param data_offset Offset from m_head[0] in bytes
	*/
	void setMAC(string mac, unsigned int data_offset);
	/** \brief Converts an integer to hexadecimal
		\param i Integer you want to convert
		\return String representing the integer as hex
	*/
	static string intToHex(unsigned int i);
	/** \brief Converts hexadecimal to decimal
		\param hex hexadecimal string, maximum 1 byte
		\return decimal representation of given hex
	*/
	unsigned char hexToInt(string hex);
	unsigned char * m_head; ///< holds the 14 byte header
};

}
#endif // ETHERNET_HEADER