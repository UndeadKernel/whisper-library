// Author: Martin Oehler
#include <bitSetEncoder.hpp>
#include <iostream>

using namespace std;
namespace whisper_library {

vector<bitset<6>> BitSetEncoder::encodeMessage(string message) {
	vector<bitset<6>> ret_vector;
	if (message.length() <= 48) {
		ret_vector = encodeMessageHelper(message);
	}
	else {
		string head = message.substr(0, 48);
		string rest = message.substr(48, message.length() - 48);
		ret_vector = encodeMessageHelper(head);
		vector<bitset<6>> rec_vector = encodeMessage(rest);
		ret_vector.insert(ret_vector.end(), rec_vector.begin(), rec_vector.end());
	}
	return ret_vector;
}

vector<bitset<6>> BitSetEncoder::encodeMessageHelper(string message) {
	string bit_sequence = "";
	vector<bitset<6>> ret_vector;

	// do nothing if the message is empty
	if (message.length() == 0) {
		return ret_vector;
	}
	// convert chars to binary
	for (unsigned int i = 0; i < message.length(); ++i) {
		bitset<8> bit(message[i]);
		bit_sequence += bit.to_string();
	}
	// push empty bitset as placeholder
	ret_vector.push_back(bitset<6>());
	//split binary
	unsigned int i = 0;
	while (i + 6 <= bit_sequence.length()) {
		bitset<6> bit(bit_sequence.substr(i, 6));
		ret_vector.push_back(bit);

		i += 6;
	}
	int numb_packets = i / 6;
	// build trailing zeroes
	string zeroes = "";
	for (unsigned int i = 0; i < bit_sequence.length() - i; i++) {
		zeroes += "0";
	}
	//create block with packet-count
	ret_vector[0] = bitset<6>(numb_packets);
	// add trailing zeroes
	ret_vector.push_back(bitset<6>(bit_sequence.substr(i, bit_sequence.length() - i) + zeroes));
	return ret_vector;
}
}