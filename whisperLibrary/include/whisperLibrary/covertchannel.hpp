/*	<covertchannel.hpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
							Simon Kadel
							Martin Sven Oehler
							Arne Sven Stühlmeyer

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef COVERT_CHANNEL
#define COVERT_CHANNEL

#include "genericpacket.hpp"
#include <string>


namespace whisper_library {


/** \brief The interface used by all covert channels

	CovertChannel is the abstract class all covert channels you want to use have to inherit from. It defines the
	interface for a covert channel. You can add CovertChannel objects to the ChannelManager to use them.
*/
class CovertChannel {
public:	
	/** \brief Constructor

		Creates a CovertChannel
	*/ 
	CovertChannel() {};
	/** \brief Destructor

		Virtual destructor so that the destructor of derived classes is called
	*/ 
	virtual ~CovertChannel() {};
	/** \return an instance of this covert channel
	*/
	virtual CovertChannel* instance();
	/** \brief sends a message

		Call this function to send a message using the covert channel.
	*/ 
	virtual void sendMessage(std::string message) = 0;	
	/** \brief receives a packet

		This function is called, when a new packet arrived. The argument is a reference to this packet.
	*/ 
	virtual void receivePacket(GenericPacket& packet) = 0;	
	/** \brief configures the covert channel
		\param arguments Arguments that are given to the covert channel
	*/
	virtual void setArguments(string arguments) = 0;
	/** \return name of the covert channel
	*/ 
	virtual std::string name() const = 0;	
	/** \return some information about the covert channel
	*/ 
	virtual std::string info() const = 0;
	/** \return the used protocol (e.g. tcp)
	*/
	virtual string protocol() const = 0;
	/** \return the used port
	*/
	virtual unsigned short port() const = 0;
};
}
#endif // COVERT_CHANNEL
