#define BOOST_TEST_MODULE bitSetEncoderTest
#include <whisperLibrary\bitSetEncoder.hpp>
#include <boost/test/unit_test.hpp>

#include<string>
#include<vector>
#include<bitset>

#include <iostream>

using namespace std;

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

BOOST_AUTO_TEST_CASE(funkction_test) {
	// -----------------------------------------
	// Init
	// -----------------------------------------
	whisperLibrary::BitSetEncoder en;

	string msg = "AB";
	string expected_output = "";

	string output;

	// -----------------------------------------
	// Tests
	// -----------------------------------------

	// Common Case
	msg = "AB";
	expected_output = "010000 010100 001000";

	output = generateTestOutput(en.encodeMessage(msg));
	BOOST_CHECK_MESSAGE(!output.compare(expected_output), "Common Test failed");

	// Empty Case
	msg = "";
	expected_output = "";

	output = generateTestOutput(en.encodeMessage(msg));
	BOOST_CHECK_MESSAGE(!output.compare(expected_output), "Empty Test failed");
}
