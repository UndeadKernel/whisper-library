//author: Simon Kadel


#include <whisperLibrary\socketconnector.hpp>
#include <whisperLibrary\bitSetEncoder.hpp>

#include <iostream>

//test - encoder
#include <string>
#include <bitset>
#include <vector>

using namespace std;

int main(int argc, char* argv[]){


	// Test -  Encoder
	whisperLibrary::BitSetEncoder en;

	vector<bitset<6>> vec;
	vec = en.encodeMessage("AB");

	string out = "";

	for (unsigned int i = 0; i < vec.size(); ++i) {
		out += vec[i].to_string() + " ";
	}

	cout << out << endl;
}