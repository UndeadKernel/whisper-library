
#ifndef COVERT_CHANNEL
#define COVERT_CHANNEL

#include <encoder.hpp>
#include <decoder.hpp>
#include <common.hpp>

namespace whisper_library {

/*
	CovertChannel is the abstract class all covert channels you want use have to inherit from. It defines the
	interface for a covert channel. You can add CovertChannel objects to the ChannelManager to use them.
*/
class WHISPERAPI CovertChannel {

public:
	// constructor
	CovertChannel();
	// prepares the message for sending using the current encoder
	void encode(std::string message);
	// reassembles the received bits to get the message
	// TODO add parameter, maybe vector of bitset?
	std::string decode();
	// --- Getter/Setter ---
	// TODO needed?
	int getProtokol();
	void setEncoder(Encoder encoder);
	void setDecoder(Decoder decoder);
	Encoder encoder() const;
	Decoder decoder() const;

private:
	// TODO needed?
	// the current Encoder
	Encoder* m_encoder;
	// the current Decoder
	Decoder* m_decoder;
};
}
#endif // COVERT_CHANNEL