//author: Simon Kadel

#include <whisper-library\socketconnector.hpp>
#include <whisper-library\encoder.hpp>
#include <iostream>

//test - encoder
#include <string>
#include <bitset>
#include <vector>

using namespace std;

int main(int argc, char* argv[]){
	whisperLibrary::SocketConnector sc = whisperLibrary::SocketConnector();
	std::cout << sc.getPort() << std::endl;

	// Test -  Encoder
	whisperLibrary::Encoder en = whisperLibrary::Encoder();

	vector<bitset<6>> vec;
	vec = en.encodeMessage("AB");

	string out = "";

	for (unsigned int i = 0; i < vec.size(); ++i) {
		out += vec[i].to_string() + " ";
	}

	cout << out << endl;
}