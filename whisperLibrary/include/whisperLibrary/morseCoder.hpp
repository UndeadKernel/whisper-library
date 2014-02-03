// 	Author: Martin Oehler

#ifndef MORSE_CODER
#define MORSE_CODER

#include <vector>
#include <boost/bimap.hpp>
#include <iostream>

using namespace std;

typedef boost::bimap<char, string> morse_map;
typedef morse_map::value_type morse;

namespace whisper_library {
	class MorseCoder {
	public:
		MorseCoder(float delay_short, float delay_long, float delay_letter, float delay_space);

		vector<float> encodeMessage(string message);

		string decodeMessage(vector<float> delays);

		vector<char> checkString(string message);

	private:
		vector<bool> encodeLetter(char letter);
		char decodeLetter(vector<bool> morse_code);
		const float m_delay_short;
		const float m_delay_long;
		const float m_delay_letter;
		const float m_delay_space;
		morse_map m_morse_map;
	};
}

#endif //MORSE_CODER