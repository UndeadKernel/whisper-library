// Author: Martin Oehler
#include <bitSetEncoder.hpp>

using namespace std;
namespace whisper_library {

vector<bitset6> BitSetEncoder::encodeMessage(string message) {
	string bit_sequence = "";
	vector<bitset6> vector;

	// do nothing if the message is empty
	if (message.length() == 0) {
		return vector;
	}
	// convert chars to binary
	for (unsigned int i = 0; i < message.length(); ++i) {
		bitset<8> bit(message[i]);
		bit_sequence += bit.to_string();
	}
	//split binary
	unsigned int i = 0;
	while (i + 6 <= bit_sequence.length()) {
		bitset<6> bit(bit_sequence.substr(i, 6));
		vector.push_back(bit);

		i += 6;
	}
	// build trailing zeroes
	string zeroes = "";
	for (unsigned int i = 0; i < bit_sequence.length() - i; i++) {
		zeroes += "0";
	}
	// add trailing zeroes
	vector.push_back(bitset<6>(bit_sequence.substr(i, bit_sequence.length() - i) + zeroes));
	return vector;
}
}
