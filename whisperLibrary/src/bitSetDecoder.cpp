// Author: Martin Oehler
#include <bitSetDecoder.hpp>

using namespace std;
namespace whisper_library {
	
string BitSetDecoder::decodeMessage(vector<bitset6> vector) {
	string bit_sequence = "";
	// append bits to string
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bit_sequence += vector[i].to_string();
	}

	// convert binary string to chars
	string message = "";
	for (unsigned int i = 0; i + 8 <= bit_sequence.length(); i += 8) {
		bitset<8> bitset(bit_sequence.substr(i, 8));
		char c = static_cast<char>(bitset.to_ulong());
		message.push_back(c);
	}
	return message;
}
}
