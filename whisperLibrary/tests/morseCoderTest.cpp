#include "morseCoder.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>


using namespace std;

struct MorseCoderFixture {

};

BOOST_FIXTURE_TEST_SUITE(morseCoder, MorseCoderFixture)

BOOST_AUTO_TEST_CASE(morse_encode_word) {
	string word = "Hello";

	whisper_library::MorseCoder morsecoder(0.1, 0.3, 0.7, 1);
	vector<float> delays = morsecoder.encodeMessage(word);
	string word_decoded = morsecoder.decodeMessage(delays);

	BOOST_CHECK_EQUAL(boost::to_upper_copy(word), word_decoded);
}

BOOST_AUTO_TEST_CASE(morse_encode_sentence) {
	string word = "Hello, this is a test.";

	whisper_library::MorseCoder morsecoder(0.1, 0.3, 0.7, 1);
	vector<float> delays = morsecoder.encodeMessage(word);
	string word_decoded = morsecoder.decodeMessage(delays);

	BOOST_CHECK_EQUAL(boost::to_upper_copy(word), word_decoded);
}

BOOST_AUTO_TEST_CASE(morse_encode_unsupported_symbols) {
	string word = "Hellô, á";

	whisper_library::MorseCoder morsecoder(0.1, 0.3, 0.7, 1);
	vector<float> delays = morsecoder.encodeMessage(word);
	string word_decoded = morsecoder.decodeMessage(delays);

	string result = "HELL#, #";
	BOOST_CHECK_EQUAL(result, word_decoded);
}

BOOST_AUTO_TEST_SUITE_END()