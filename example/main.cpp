#include <whisperLibrary\socketconnector.hpp>
#include <whisperLibrary\bitSetCoder.hpp>

#include <iostream>

//test - encoder
#include <string>
#include <bitset>
#include <vector>

using namespace std;

int main(int argc, char* argv[]){
	whisper_library::BitSetCoder<6> coder;
	cout << "Block size: " << coder.blockLength() << endl;

	// Test -  Encoder
	vector<bitset<6>> vec;
	string msg = "AAA";
	cout << "Message: " << msg << endl;
	vec = coder.encodeMessage(msg);
	string out = "";
	for (unsigned int i = 0; i < vec.size(); ++i) {
		out += vec[i].to_string() + " ";
	}
	cout << "Encoder: " << out << endl;

	// test - Decoder
	string msg_d = coder.decodeMessage(vec);
	cout << "Decoder: " << msg_d << endl;
}