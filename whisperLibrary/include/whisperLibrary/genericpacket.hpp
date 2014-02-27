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

	class GenericPacket {
	public:
		GenericPacket() {};
		GenericPacket(vector<bool> content) : m_content(content) {};
		void setContent(vector<bool> content);
		vector<bool> content();
	private:
		vector<bool> m_content;
	};

}

#endif // GENERIC_PACKET