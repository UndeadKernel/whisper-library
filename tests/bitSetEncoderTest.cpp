#include <whisperLibrary\bitSetEncoder.hpp>
#include <boost/test/unit_test.hpp>

#include<string>
#include<vector>
#include<bitset>

using namespace std;

struct BitSetEncoderFixture {
	BitSetEncoderFixture(){
		en = whisper_library::BitSetEncoder();
	}
	~BitSetEncoderFixture(){
	}
	string msg;
	string expected_output;
	string output;
	whisper_library::BitSetEncoder en;
};

string generateTestOutput(vector<bitset<6>> vec) {
	string ret = "";
	for (unsigned int i = 0; i < vec.size(); ++i) {
		ret += vec[i].to_string();

		// add whitespace between elements, if not last element
		if (i + 1 != vec.size()) {
			ret += " ";
		}
	}

	return ret;
}

BOOST_FIXTURE_TEST_SUITE(bitSetEncoder, BitSetEncoderFixture)

BOOST_AUTO_TEST_CASE(encode_simple_message) {

	msg = "AB";
	expected_output = "010000 010100 001000";

	output = generateTestOutput(en.encodeMessage(msg));
	BOOST_CHECK_EQUAL(output, expected_output);
}
BOOST_AUTO_TEST_CASE(encode_empty_message) {

	msg = "";
	expected_output = "";

	output = generateTestOutput(en.encodeMessage(msg));
	BOOST_CHECK_EQUAL(output, expected_output);
}

BOOST_AUTO_TEST_SUITE_END()