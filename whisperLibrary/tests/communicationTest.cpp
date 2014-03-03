#include <channelmanager.hpp>
#include <thread>

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

struct CommunicationFixture {
	CommunicationFixture() {
		channelmanager.setErrorStream(&cout);
		channelmanager.setOutputStream(&cout);
		adapter_name = "";
	}
	whisper_library::ChannelManager channelmanager;
	vector<char*> last_adapters;
	char* adapter_name;

	int processCommand(string command) {
		if (command.compare("help") == 0) {
			printCommands();
			return 0;
		}
		if (command.compare("exit") == 0) {
			return 2;
		}
		if (command.compare("displayCC") == 0) {
			printCovertChannels();
			return 0;
		}
		size_t found = command.find("selectCC");
		if (found != string::npos) {
			int index = 0;
			try {
				index = boost::lexical_cast<int>(command.substr(9, command.length() - 9));
			}
			catch (boost::bad_lexical_cast e) {
				cout << "Parameter war keine Zahl." << endl;
				return 0;
			}

			channelmanager.selectChannel(index);
			cout << "Selected channel: " << channelmanager.currentChannel() << endl;
			return 0;
		}
		if (command.compare("displayAdapters") == 0) {
			printAdapters();
			return 0;
		}
		found = command.find("selectAdapter");
		if (found != string::npos) {
			int index = 0;
			try {
				index = boost::lexical_cast<int>(command.substr(14, command.length() - 14));
			}
			catch (boost::bad_lexical_cast e) {
				cout << "Parameter war keine Zahl." << endl;
				return 0;
			}
			selectAdapterId(index);
			cout << "Selected adapter: " << adapter_name << endl;
			return 0;
		}
		found = command.find("connect");
		if (found != string::npos) {
			if (adapter_name == "") {
				cout << "Kein Adapter ausgewählt" << endl;
				return 0;
			}
			string ip_port = command.substr(8, command.length() - 8);
			vector<string> parts;
			boost::split(parts, ip_port, boost::is_any_of(":"), boost::token_compress_on);
			if (parts.size() == 2) {
				short port;
				try {
					port = boost::lexical_cast<short>(parts[1]);
				}
				catch (boost::bad_lexical_cast e) {
					cout << "Port war keine Zahl." << endl;
					return 0;
				}
				cout << "Trying to open a connection" << endl;
				channelmanager.openConnection(parts[0], port, adapter_name);
				cout << "Opened connection to " << ip_port << endl;
				return 1;
			}
			else {
				cout << "Eingabe der Zieladresse in der Form: IP:Port.";
				return 0;
			}

		}
		cout << "Command '" << command << "' unknown." << endl;
		return 0;
	}

	int processMessage(string message) {
		if (message.compare("exit") == 0) {
			return 1;
		}
		if (message.compare("help") == 0) {
			cout << "Help not available in chat mode. Close chat first with 'exit'" << endl;
			return 0;
		}
		channelmanager.sendMessage(message);
		return 0;
	}

	void printCovertChannels() {
		vector<string> names = channelmanager.getChannelNames();
		vector<string> infos = channelmanager.getChannelInfos();
		for (int i = 0; i < names.size(); i++) {
			cout << "[" << i << "] " << names[i] << ": " << infos[i] << endl;
		}
	}

	void printAdapters() {
		vector<char*> names = channelmanager.adapterNames();
		last_adapters = names;
		vector<char*> infos = channelmanager.adapterDescriptions();
		for (int i = 0; i < names.size(); i++) {
			cout << "[" << i << "] " << names[i] << ": " /*<< infos[i]*/ << endl;
		}
	}

	void selectAdapterId(unsigned int index) {
		if (!last_adapters.empty()) {
			if (index < last_adapters.size()) {
				adapter_name = last_adapters[index];
			}
			else {
				cout << "Ungültiger Parameter" << endl;
			}

		}
		else {
			vector<char*> names = channelmanager.adapterNames();
			if (index < names.size()) {
				adapter_name = names[index];
			} else {
				cout << "Ungültiger Parameter" << endl;
			}
		}
	}

	void printCommands() {
		cout << "Available commands: " << endl;
		cout << "displayCC: Displays a list of available Covert Channels" << endl;
		cout << "displayAdapters: Displays a list of available network adapters" << endl;
		cout << "selectCC [ID]: Select the covert channel with ID [ID]" << endl;
		cout << "selectAdapter [ID]: Select the network adapter with ID [ID]" << endl;
		cout << "connect [IP:Port]: Connect to IP:Port using the selected Covert Channel and adapter." << endl;
		cout << "exit: Exit the chat/program" << endl;
		cout << "help: Print this list again" << endl;
		cout << endl;
	}

};

BOOST_FIXTURE_TEST_SUITE(communication, CommunicationFixture)

BOOST_AUTO_TEST_CASE(basic_communication) {
	cout << "Whisper Library Test" << endl << endl;
	printCommands();
	cout << "Enter your command:" << endl;
	string command;
	int code = 0;
	while (code != 2) {
		std::getline(cin, command);
		code = processCommand(command);
		if (code == 1) {
			// chat
			cout << "chat started.." << endl;
			string message;
			int chatCode = 0;
			while (chatCode != 1) {
				std::getline(cin, message);
				chatCode = processMessage(message);
			}
			cout << "chat ended." << endl;
		}
	}



}

BOOST_AUTO_TEST_SUITE_END()
