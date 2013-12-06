#ifndef COVERT_CHANNEL
#define COVERT_CHANNEL

#include <encoder.hpp>
#include <decoder.hpp>
#include <common.hpp>

namespace whisperLibrary {

class WHISPERAPI CovertChannel {

public:
	CovertChannel();
	int getProtokol();
	void encode(std::string msg);
	void decode();
	void setEncoder(Encoder encoder);
	void setDecoder(Decoder decoder);

private:
	Encoder encoder;
	Decoder decoder;
};

}


#endif