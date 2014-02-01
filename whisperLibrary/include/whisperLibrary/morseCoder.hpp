// 	Author: Martin Oehler

#ifndef MORSE_CODER
#define MORSE_CODER

#include <vector>
#include "boost/bimap.hpp"

using namespace std;

namespace whisper_library {
	class MorseCoder {
	public:
		vector<float> encodeMessage(string message);

		string decodeMessage(vector<float> delays);

		vector<int> encodeLetter(char letter);

	private:
		float m_delay_short;
		float m_delay_long;
		float m_delay_letter;
		float m_delay_space;
	};
}

#endif //MORSE_CODER