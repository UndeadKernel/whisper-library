// 	Author: Martin Oehler

#include <morseCoder.hpp>

namespace whisper_library {
	vector<float> MorseCoder::encodeMessage(string message) {

	}

	vector<int> encodeLetter(char letter) {
		typedef boost::bimap<char, string> map_type;
		map_type map;
		map.insert(map_type::value_type('A', "01"));
		map.insert(map_type::value_type('B', "1000"));
		map.insert(map_type::value_type('C', "1010"));

		char uppercase_letter = toupper(letter);

		string encoding = map.left.at(uppercase_letter);

		vector<int> encoding_vector;
		for (int i = 0; i < encoding.length(); i++) {
			encoding_vector.push_back(atoi(encoding[i] + ""));
		}
		return encoding_vector;
	}

	string MorseCoder::decodeMessage(vector<float> delays) {

	}
}