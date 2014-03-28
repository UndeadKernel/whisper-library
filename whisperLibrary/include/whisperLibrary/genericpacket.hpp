/*	<genericpacket.hpp>
	Copyright(C) 2014	Jan Simon Bunten
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
#ifndef GENERIC_PACKET
#define GENERIC_PACKET

#include <vector>

using namespace std;
namespace whisper_library {
/** \brief Represents a network packet
*/
class GenericPacket {
public:
	/** \brief Creates a GenericPacket
	*/
	GenericPacket() {};
	/** \brief Creates a GenericPacket with the given content
		\param content The bits used to fill the packet
	*/
	GenericPacket(vector<bool> content) : m_content(content) {};
	/** \brief Sets the content of the packet
		\param content The new content
	*/
	void setPacket(vector<bool> content);
	/** \return the packets content
	*/
	vector<bool> packet() const;
private:
	vector<bool> m_content;///< holds the packets content
};

}

#endif // GENERIC_PACKET