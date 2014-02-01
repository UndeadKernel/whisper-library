// 	Author: Martin Oehler

#include <morseCoder.hpp>

namespace whisper_library {
	vector<float> MorseCoder::encodeMessage(string message) {
		vector<float> delays;
		for (int i = 0; i < message.length(); i++) {
			if (message[i] == ' ') {
				delays.push_back(m_delay_space);
			}
			else {
				vector<bool> encoded_letter = encodeLetter(message[i]);

				for (vector<bool>::iterator it = encoded_letter.begin(); it != encoded_letter.end(); it++) {
					if ((*it)) {
						delays.push_back(m_delay_long);
					}
					else {
						delays.push_back(m_delay_short);
					}
				}
				if (i + 1 != message.length()) {
					delays.push_back(m_delay_letter);
				}
			}
		}

		return delays;
	}

	vector<bool>  MorseCoder::encodeLetter(char letter) {
		typedef boost::bimap<char, string> map_type;
		map_type map;
		map.insert(map_type::value_type('A', "01"));
		map.insert(map_type::value_type('B', "1000"));
		map.insert(map_type::value_type('C', "1010"));

		char uppercase_letter = toupper(letter);
		cout << uppercase_letter << endl;

		string encoding = map.left.at(uppercase_letter);
		vector<bool> encoding_vector;
		for (int i = 0; i < encoding.length(); i++) {
			encoding_vector.push_back(encoding[i] == '1');
		}
		return encoding_vector;
	}

	string MorseCoder::decodeMessage(vector<float> delays) {
		return "";
	}
}