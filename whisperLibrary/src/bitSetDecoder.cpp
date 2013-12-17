// Author: Martin Oehler

#include <bitSetDecoder.hpp>

using namespace std;

namespace whisper_library {
	
string BitSetDecoder::decodeMessage(vector<bitset<6>> vec) {
	string bitstream = "";

	// append bits to string
	for (unsigned int i = 0; i < vec.size(); ++i) {
		bitstream += vec[i].to_string();
	}

	// convert binary string to chars
	string msg = "";
	for (unsigned int i = 0; i+8 <= bitstream.length(); i += 8) {
		bitset<8> bs(bitstream.substr(i, 8));
		char c = static_cast<char>(bs.to_ulong());
		msg.push_back(c);
	}

	return msg;
}

}