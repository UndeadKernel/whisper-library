//author: Jan Simon Bunten

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
 */
class TcpPacket {

public:
    /** 
     * \brief The empty constructor creates an empty tcp packet.
     */
    TcpPacket();
	
    /**
     * \brief This constructor is used to fill in some of the important fields beforehand.
     * \param inSourcePort the source port as integer
     * \param inDestPort the destination port as integer
     * \param inSequenceNumber the sequence number of the packet
     * \param inAckNumber the acknowladgement number of the packet
     * \param inDataOffset the length of the options field in bytes
     * \param inWindowSize the size of the packet in bytes
     * \param inOptions the options as a vector of boolean values
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
    ulong acknowlageNumber() const;
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

    // start of the setters
	void setSourcePort(uint val);
	void setDestPort(uint val);
	void setSequenceNumber(ulong val);
	void setAcknowlageNumber(ulong val);
	void setDataOffset(bitset<4> val);
	void setReserved(bitset<3> val);
    void setFlags(bitset<9> val);
    void setNonceSumFlag(bool val);
    void setCongestionWindowReducedFlag(bool val);
    void setEcnEchoFlag(bool val);
    void setUrgentFlag(bool val);
    void setAcknowledgementFlag(bool val);
    void setPushFlag(bool val);
    void setResetFlag(bool val);
    void setSynchronisationFlag(bool val);
    void setFinishFlag(bool val);
	void setWindowSize(uint val);
	void setChecksum(uint val);
    void setUrgentPointer(uint val);
    void setOptions(vector<bool> val);
    void setData(vector<bool> val);
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
    ulong vectorToULong(int start, int end, const vector<bool> &vec) const;

    /** 
		\brief This function is used to insert an unsigned integer value into a given boolean vector.
		\param start The index where the first bit will be inserted
		\param end The index where the last bit will be inserted
		\param vec The vector the value will be inserted in
		\param val The value to be inserted
		\tparam T The type of value to be inserted into the vector
    */
    template <class T> void uIntToVector(int start, int end, vector<bool> &vec, T val);

    /** 
     * \brief This function is used to convert a given integer value into a boolean vector.
     * \param val The value to be converted
     * \return A vector of booleans with a binary representation of the value
     * \tparam T The type of the value to be converted
     */
    template <class T> vector<bool> intToBoolVector(T val);

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
	vector<bool> trimBigEndianVector(vector<bool> vec, int size);


	vector<bool> m_header;
    vector<bool> m_options;
    vector<bool> m_data;	
};
}
#endif
