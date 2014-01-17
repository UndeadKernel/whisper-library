#include <whisperLibrary/socketconnector.hpp>
#include <whisperLibrary/bitSetEncoder.hpp>
#include <whisperLibrary/bitSetDecoder.hpp>

#include <iostream>

//test - encoder
#include <string>
#include <bitset>
#include <vector>

using namespace std;
typedef bitset<6> bitset6;

int main(int argc, char* argv[]){


	// Test -  Encoder
	whisper_library::BitSetEncoder en;

	vector<bitset6> vec;
	string msg = "123 456 !+#";
	cout << "Message: " << msg << endl;
	vec = en.encodeMessage(msg);

	string out = "";

	for (unsigned int i = 0; i < vec.size(); ++i) {
		out += vec[i].to_string() + " ";
	}

	cout << "Encoder: " << out << endl;

	// test - Decoder

	whisper_library::BitSetDecoder dec;

	string msg_d = dec.decodeMessage(vec);

	cout << "Decoder: " << msg_d << endl;
}
