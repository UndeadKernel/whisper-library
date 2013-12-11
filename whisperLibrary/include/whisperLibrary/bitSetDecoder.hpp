// Class that converts the message, that we want to send, to a format, that our covert channel can use.
// Author: Martin Oehler

#ifndef BITSET_DECODER
#define BITSET_DECODER

#include "common.hpp"

#include <string>
#include <bitset>
#include <vector>

using namespace std;

namespace whisperLibrary {

	class WHISPERAPI BitSetDecoder {

	public:
		// 
		// 
		// INPUT:
		//			vector<bitset<6>> -> A vector containing bitsets. The bitsets contain the binary representation of ASCI-encoded letters.
		//								 To reconvert the message, you have to append the bitsets in order (0-n). 
		// OUTPUT:
		//			string msg -> The message that we want to split
		//								 
		string decodeMessage(vector<bitset<6>> vec);

	private:

	};

}

#endif