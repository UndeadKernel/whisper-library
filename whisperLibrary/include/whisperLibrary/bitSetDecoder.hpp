// Author: Martin Oehler

#ifndef BITSET_DECODER
#define BITSET_DECODER

#include "common.hpp"
#include <string>
#include <bitset>
#include <vector>

using namespace std;

namespace whisper_library {

/*
	BitSetDecoder reassembles the message blocks of 6 bit, that we received through a covert channel.
	To do this, call the function 'decodeMessage' with the message blocks as a vector.
*/
class WHISPERAPI BitSetDecoder {

public:
	/*
		decodeMessage takes 6-bit blocks and converts them to characters returned as a string.
		The bitsets contain the binary representation of ASCI-encoded letters.
	*/
	string decodeMessage(vector<bitset<6>> vector);
};

}

#endif // BITSET_DECODER