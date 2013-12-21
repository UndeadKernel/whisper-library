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
	BitSetDecoder reassembles the message blocks, that we received through a TCP Header covert channel.
*/
class WHISPERAPI BitSetDecoder {

	public:
		/*
			decodeMessage takes 6-bit blocks and converts them to chars returned as a string.
			vector<bitset<6>> is generated by BitSetEncoder.encodeMessage(string msg).
			The bitsets contain the binary representation of ASCI-encoded letters.
		*/
		string decodeMessage(vector<bitset<6>> vec);
};

}

#endif // BITSET_DECODER