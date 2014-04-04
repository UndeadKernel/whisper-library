#include <channelmanager.hpp>
#include <thread>

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

struct CommunicationFixture {
	CommunicationFixture() {
		channelmanager.setErrorStream(&cout);
		channelmanager.setOutputStream(&cout);
		destination_ip = "";
		channel_id = channelmanager.getChannelIDs()[0];
	}
	whisper_library::ChannelManager channelmanager;
	vector<string> last_adapters;
	string destination_ip;
	string channel_id;

	int processCommand(string command) {
		vector<string> arguments;
		boost::split(arguments, command, boost::is_any_of(" "), boost::token_compress_on);
		if (arguments.size() > 2) {
			cout << "Too many arguments." << endl;
			return 0;
		}

		if (arguments[0].compare("help") == 0) {
			printCommands();
			return 0;
		}
		if (arguments[0].compare("exit") == 0) {
			return 2;
		}
		if (arguments[0].compare("displayCC") == 0) {
			printCovertChannels();
			return 0;
		}
		if (arguments[0].compare("selectCC") == 0) {
			string id = arguments[1];
			for (unsigned int i = 0; i < channelmanager.channelCount(); i++){
				if (channelmanager.getChannelIDs()[i].compare(id) == 0) {
					channel_id = id;
					return 0;
				}
			}
			cout << "Selected channel not found" << endl;
			return 0;
		}
		if (arguments[0].compare("displayAdapters") == 0) {
			printAdapters();
			return 0;
		}
		if (arguments[0].compare("selectAdapter") == 0) {
			int index = 0;
			try {
				index = boost::lexical_cast<int>(arguments[1]);
			}
			catch (boost::bad_lexical_cast e) {
				cout << "Parameter was not a number" << endl;
				return 0;
			}
			selectAdapterId(index);
			return 0;
		}
		if (arguments[0].compare("connect") == 0) {
			cout << "Trying to open a connection" << endl;
			bool success = channelmanager.openConnection(arguments[1], channel_id);
			if (success) {
				destination_ip = arguments[1];
				cout << "Opened connection to " << arguments[1] << endl;
				return 1;
			}
			else {
				return 0;
			}

		}
		cout << "Command '" << command << "' unknown." << endl;
		return 0;
	}

	int processMessage(string message) {
		if (message.compare("exit") == 0) {
			channelmanager.closeConnection(destination_ip);
			return 1;
		}
		vector<string> arguments;
		boost::split(arguments, message, boost::is_any_of(" "), boost::token_compress_on);
		if (arguments[0].compare("argument") == 0) {
			if (arguments.size() < 2) {
				cout << "Not enough arguments" << endl;
				return 0;
			}
			string argument_list = arguments[1];
			for (unsigned int i = 2; i < arguments.size(); i++) {
				argument_list += " " + arguments[i];
			}
			channelmanager.setChannelArguments(destination_ip, argument_list);
			cout << "Set channel argument to '" << argument_list << "'." << endl;
			return 0;
		}
		if (message.compare("help") == 0) {
			cout << "Help not available in chat mode. Close chat first with 'exit'" << endl;
			return 0;
		}
		channelmanager.sendMessage(destination_ip, message);
		return 0;
	}

	void printCovertChannels() {
		vector<string> ids = channelmanager.getChannelIDs();
		vector<string> names = channelmanager.getChannelNames();
		vector<string> infos = channelmanager.getChannelInfos();
		for (unsigned int i = 0; i < names.size(); i++) {
			cout << "[" << ids[i] << "] " << names[i] << ": " << infos[i] << endl << endl;
		}
	}

	void printAdapters() {
		vector<string> names = channelmanager.networkAdapters();
		last_adapters = names;
		for (unsigned int i = 0; i < names.size(); i++) {
			cout << "[" << i << "] " << names[i] << ": " << channelmanager.adapterDescription(names[i]) << endl;
		}
	}

	void selectAdapterId(unsigned int index) {
		if (last_adapters.empty()) {
			last_adapters = channelmanager.networkAdapters();
		}
		if (index < last_adapters.size()) {
			channelmanager.setAdapter(last_adapters[index]);
			vector<string> addresses = channelmanager.adapterAddresses();
			cout << "Addresses: " << endl;
			for (unsigned int i = 0; i < addresses.size(); i++) {
				cout << addresses[i] << endl;
			}
			cout << endl;
		}
		else {
			cout << "Invalid parameter" << endl;
		}
	}

	void printCommands() {
		cout << "Available commands: " << endl;
		cout << "displayCC: Displays a list of available Covert Channels" << endl;
		cout << "displayAdapters: Displays a list of available network adapters" << endl;
		cout << "selectCC [ID]: Select the covert channel with ID [ID]" << endl;
		cout << "argument [string]: Gives [string] as an argument to the channel. Only usable during chat" << endl;
		cout << "selectAdapter [ID]: Select the network adapter with ID [ID]" << endl;
		cout << "connect [IP]: Connect to IP using the selected Covert Channel and adapter." << endl;
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
