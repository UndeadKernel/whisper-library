#include "morseCoder.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>


using namespace std;

struct MorseCoderFixture {
	string message;
	string message_decoded;
};

BOOST_FIXTURE_TEST_SUITE(morseCoder, MorseCoderFixture)

BOOST_AUTO_TEST_CASE(morse_encode_word) {
	message = "Hello";

	whisper_library::MorseCoder morsecoder(1, 3, 7, 10);
	vector<unsigned int> delays = morsecoder.encodeMessage(message);
	message_decoded = morsecoder.decodeMessage(delays);

	BOOST_CHECK_EQUAL(boost::to_upper_copy(message), message_decoded);
}

BOOST_AUTO_TEST_CASE(morse_encode_sentence) {
	message = "Hello, this is a test.";

	whisper_library::MorseCoder morsecoder(1, 3, 7, 10);
	vector<unsigned int> delays = morsecoder.encodeMessage(message);
	message_decoded = morsecoder.decodeMessage(delays);

	BOOST_CHECK_EQUAL(boost::to_upper_copy(message), message_decoded);
}

BOOST_AUTO_TEST_CASE(morse_encode_unsupported_symbols) {
	message = "HellÙ, ·";

	whisper_library::MorseCoder morsecoder(1, 3, 7, 10);
	vector<unsigned int> delays = morsecoder.encodeMessage(message);
	message_decoded = morsecoder.decodeMessage(delays);

	string result = "HELL#, #";
	BOOST_CHECK_EQUAL(result, message_decoded);
}

BOOST_AUTO_TEST_CASE(morse_check_unsupported_symbols) {
	message = "DÓes ist ‚ ein TÈst!";
	string check_unsupported_symbols = "Ó‚È!";

	whisper_library::MorseCoder morsecoder(1, 3, 7, 10);
	vector<char> vec_unsupported_symbols = morsecoder.checkString(message);
	string unsupported_symbols = "";
	for (vector<char>::iterator it = vec_unsupported_symbols.begin(); it != vec_unsupported_symbols.end(); it++) {
		unsupported_symbols += (*it);
	}
	BOOST_CHECK_EQUAL(check_unsupported_symbols, unsupported_symbols);

}

BOOST_AUTO_TEST_SUITE_END()