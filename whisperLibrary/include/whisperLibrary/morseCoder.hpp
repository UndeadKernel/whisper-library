// 	Author: Martin Oehler

#ifndef MORSE_CODER
#define MORSE_CODER

#include <vector>
#include <boost/bimap.hpp>

using namespace std;

namespace whisper_library {
	class MorseCoder {
	public:
		MorseCoder(float delay_short, float delay_long, float delay_letter, float delay_space) :
			m_delay_short(delay_short),
			m_delay_long(delay_long),
			m_delay_letter(delay_letter),
			m_delay_space(delay_space) {};

		vector<float> encodeMessage(string message);

		string decodeMessage(vector<float> delays);

		// make privat
		vector<bool> encodeLetter(char letter);

	private:
		float m_delay_short;
		float m_delay_long;
		float m_delay_letter;
		float m_delay_space;
	};
}

#endif //MORSE_CODER