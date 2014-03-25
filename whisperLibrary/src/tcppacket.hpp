/*	<tcppacket.hpp>
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
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TCP_PACKET
#define TCP_PACKET

#include "../include/whisperLibrary/common.hpp"
#include <vector>
#include <bitset>
#include <algorithm>

using namespace std;
typedef unsigned int uint;
typedef unsigned long ulong;

namespace whisper_library {

/** 
 * \brief This class represents a tcp packet. All fields are accessable.
 * 	
 * 	All the different fields of the TCP packet can be accessed using getter and setter 
 * 	methods. To make the TCP packet valid, you must invoke calculate checksum first, after
 * 	setting all the fields. Keep in mind that
 * 	all information that is passed in binary format uses big endian notation.
 */
class TcpPacket {

public:
    /** 
     * \brief The empty constructor creates an empty tcp packet. Keep in mind that
     * 	all information that is passed in binary format uses big endian notation.
     */
    TcpPacket();
	
    /**
     * \brief This constructor is used to fill in some of the important fields beforehand.
     * \param inSourcePort The source port as integer. Max value 2^16.
     * \param inDestPort The destination port as integer. Max value 2^16.
     * \param inSequenceNumber The sequence number of the packet. Max value 2^32.
     * \param inAckNumber The acknowledgement number of the packet. Max value 2^32.
     * \param inDataOffset The length of the options field in bytes. Max value 2^4.
     * \param inWindowSize The size of the packet in bytes. Max value 2^16.
     * \param inOptions The options as a vector of boolean values. Max size .
     */
    TcpPacket(uint inSourcePort, 
				uint inDestPort, 
				ulong inSequenceNumber,
				ulong inAckNumber,
				bitset<4> inDataOffset,
				uint inWindowSize,
				vector<bool> inOptions);

    // start of the getters
    uint sourcePort() const;
    uint destPort() const;
    ulong sequenceNumber() const;
    ulong acknowlegeNumber() const;
    bitset<4> dataOffset() const;
    bitset<3> reserved() const;
    bitset<9> flags() const;
    bool nonceSumFlag() const;
    bool congestionWindowReducedFlag() const;
    bool ecnEchoFlag() const;
    bool urgentFlag() const;
    bool acknowledementFlag() const;
    bool pushFlag() const;
    bool resetFlag() const;
    bool synchronisationFlag() const;
    bool finishFlag() const;
    uint windowSize() const;
    uint checksum() const;
    uint urgentPointer() const;
    vector<bool> options() const;
    vector<bool> packet() const;
    vector<bool> data() const;

    /** 
     * \brief Create a packet based of a given boolean vector.
     * \param packet The packet to convert.
     */
    TcpPacket(vector<bool> packet);
    
    // start of the setters
    /** \param val The port that is used to send the packet. \n
     * 	Max value is 2^16.
     */  
	void setSourcePort(uint val);
	/** \param val The port at which the packet is received. \n
     * 	Max value is 2^16.
     */
	void setDestPort(uint val);
	/** \param val The sequence number of the packet. \n
     * 	- If the syn flag is set this is the initial sequence number. And the actual first sequence number and
     *  acknowledgement number are this number + 1.\n
     * 	- Else this is the accumulated sequence number of all the data packets based on the initial sequence number.
     */
	void setSequenceNumber(ulong val);
	/** \param val The acknowledgement number of the packet. \n
     * 	If the acknowledgement number is set, this is the next packet the reciever is expecting.\n
     *  All packets prior the acknowledgement number have been received.\n
     * 	The first ACK sent by each end acknowledges the initial sequence number, but no data.
     */
	void setAcknowlegeNumber(ulong val);
	/** \param val The size of the tcp header in 32-bit words. \n
	 *  The smallest size without options is 5.\n
	 * 	The maximum size is 15.
     */
	void setDataOffset(bitset<4> val);
	/** \param val The reserved bits of the tcp header.
	 * 	Keep in mind that this is, like all information in the header, in big endian notation.
     */
	void setReserved(bitset<3> val);
    /** \param val Set all the flags of the tcp header with one bitset.\n
     * 	Their order is: NS, CWR, ECN, URG, ACK, PSH, SYN, FIN.
     * 	For more detailed information see the corresponding setters.
     */
    void setFlags(bitset<9> val);
    /** \param val ECN-nounce concealment protection. (RFC 3540)
     */
    void setNonceSumFlag(bool val);
    /** \param val This flag is set by the sending host, to indicate that it received a
     * 	TCP segment with the ECE flag set and that it responded using a congestion control
     * 	mechanism. (RFC 3168)
     */
    void setCongestionWindowReducedFlag(bool val);
    /** \param val - If the SYN flag is set, the TCP peer is ECN capable.\n
     * 	- If the SYN flag is clear, that a packet with Congestion Experienced flag in the IP header
     * 	set is received during normal transmission (RFC 3168).
     */
    void setEcnEchoFlag(bool val);
    /** \param val Indicates, that the urgent pointer field is significant.
     */
    void setUrgentFlag(bool val);
    /** \param val Indicates, that the acknowledgement field is significant.\n
     * 	All packets after the initial SYN packet sent by the client should have this flag set.
     */
    void setAcknowledgementFlag(bool val);
    /** \param val Asks to push the buffered data to the recieving application.
     */
    void setPushFlag(bool val);
    /** \param val Reset the connection.
     */
    void setResetFlag(bool val);
    /** \param val Synchronize sequence numbers. Only the first packet sent from each end
     * 	should have this flag set. Some other flags change the meaning based on this flag
     * 	and some are only valid for when it is set and others when it´s clear.
     */
    void setSynchronisationFlag(bool val);
    /** \param val No more data from sender.
     */
    void setFinishFlag(bool val);
	/** \param val The size of the recieve window, which specifies the number of 
	 * 	window size units (by default, bytes) (beyond the sequence number in the acknowledgement field)
	 * 	that the sender of this segment is currently willing to recieve.
     */
	void setWindowSize(uint val);
	/** \param val The 16-bit checksum field is used for error checking of the header and data.
     */
	void setChecksum(uint val);
    /** \param val If the URG flag is set, then this 16-bit field is an offset from the sequence number indicating
     * 	the last urgent data byte.
     */
    void setUrgentPointer(uint val);
    /** \param val A set of options. The size has to be dividable by 32.
     */
    void setOptions(vector<bool> val);
    /** \param val Set the data of the packet.
     */
    void setData(vector<bool> val);
    /** \param val Convert a boolean vector into the packet object.
     */
    void setPacket(vector<bool> val);
        
    // other public functions
        
    /** 
		\brief This function is used to calculate the tcp checksum.
		
		To do this it needs some additional information of the IP header as parameters.
		\param sourceIp IP adress of the sender as integer
		\param destIp IP adress of the reciever as integer
		\param reservedBits the value of the reserved bits of the IP header as integer
		\param protocol the protocol used to send the data as integer value
    */
    void calculateChecksum(ulong sourceIp, ulong destIp, uint reservedBits, uint protocol);

private: 
    /** 
		\brief This function is used to convert a vector into an unsigned integer value.
		\param start the starting bit of the value in the given vector
		\param end the last bit of the value in the given vector
		\param vec the vector the value is stored in
		\return the value of the bits as integer
    */
    ulong vectorToULong(uint start, uint end, const vector<bool> &vec) const;

    /** 
		\brief This function is used to insert an unsigned integer value into a given boolean vector.
		\param start The index where the first bit will be inserted
		\param end The index where the last bit will be inserted
		\param vec The vector the value will be inserted in
		\param val The value to be inserted
		\tparam T The type of value to be inserted into the vector
    */
    template <class T> void uIntToVector(uint start, uint end, vector<bool> &vec, T val);

    /** 
     * \brief This function is used to convert a given integer value into a boolean vector.
     * \param val The value to be converted
     * \return A vector of booleans with a binary representation of the value
     * \tparam T The type of the value to be converted
     */
    template <class T> vector<bool> intToBoolVector(T val, uint size);

    /** 
		\brief This function uses the one complement addition to add two 16 bit boolean vectors.
		\param vec1 The first 16bit boolean vector to be added.
		\param vec2 The second 16bit boolean vector to be added.
		\return The 16bit one complement sum of the two given vectors.
    */ 
    vector<bool> oneComplementAdd(vector<bool> vec1, vector<bool> vec2);

    /** 
     * \brief This function splits a 32 bit boolean vector into two 16 bit vectors.
     * \param vec The vector to be split up.
     * \return A vector of two 16 bit boolean vectors.
     */
	vector<vector<bool> > split32BitVector(vector<bool> vec);

    /**
     * \brief This function splits the whole TCP packet into a vector of 16bit boolean vectors.
     * \return The TCP packet as a vector of 16bit boolean vectors.
     */
	vector<vector<bool> > splitHeaderTo16Bit();

    /** 
     * \brief This function trims a big endian boolean vector to a fixed size.
     * \param vec The vector to be trimmed.
     * \param size The new size of the vector.
     * \return The vector trimmed to the given size.
     */
	vector<bool> trimBigEndianVector(vector<bool> vec, uint size);

	/**
	 * \brief This vector stores the bits 0-159 of the TCP header. These are fields but the options.
	 */
	vector<bool> m_header;
    /**
	 * \brief This vector stores the options field of the TCP header.
	 */
    vector<bool> m_options;
    /**
	 * \brief This vector stores data carrying bits of the TCP packet.
	 */
    vector<bool> m_data;	
};
}
#endif
