#include <channelmanager.hpp>
#include <thread>

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

struct CommunicationFixture {
	CommunicationFixture() {
		channelmanager.setErrorStream(&cout);
		channelmanager.setOutputStream(&cout);
		connection = false;
	}
	whisper_library::ChannelManager channelmanager;
	vector<char*> last_adapters;
	char* adapter_name;
	bool connection;

	int processCommand(string command) {
		if (command.compare("close") == 0) {
			return 2;
		}
		if (command.compare("displayCC") == 0) {
			printCovertChannels();
			return 0;
		}
		size_t found = command.find("selectCC");
		if (found != string::npos) {
			int index = atoi(command.substr(9, command.length() - 9).c_str());
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
			int index = atoi(command.substr(14, command.length() - 14).c_str());
			selectAdapterId(index);
			cout << "Selected adapter: " << adapter_name << endl;
			return 0;
		}
		found = command.find("connect");
		if (found != string::npos) {
			connection = true;
			string ip_port = command.substr(8, command.length() - 8);
			vector<string> parts;
			boost::split(parts, ip_port, boost::is_any_of(":"), boost::token_compress_on);
			channelmanager.openConnection(parts[0], atoi(parts[1].c_str()), adapter_name);
			cout << "Opened connection to " << ip_port << endl;
			return 1;
		}
		cout << "Command '" << command << "' unknown." << endl;
		return 0;
	}

	int processMessage(string message) {
		if (message.compare("endChat") == 0) {
			return 1;
		}
		channelmanager.sendMessage(message);
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
			cout << "[" << i << "] " << names[i] << ": " << infos[i] << endl;
		}
	}

	void selectAdapterId(int index) {
		if (!last_adapters.empty()) {
			adapter_name = last_adapters[index];
		}
		else {
			adapter_name = channelmanager.adapterNames()[index];
		}
	}

};

BOOST_FIXTURE_TEST_SUITE(communication, CommunicationFixture)

BOOST_AUTO_TEST_CASE(basic_communication) {
	cout << "Whisper Library Test" << endl;
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
				cin >> message;
				chatCode = processMessage(message);
			}
			cout << "chat ended." << endl;
		}
	}



}

BOOST_AUTO_TEST_SUITE_END()