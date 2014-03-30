
#ifdef __cplusplus
extern "C" {
#endif
	struct WhisperMessage{
		char* who;
		char* message;
	};
	void wlMakeChannelManager();
	void wlDestroyChannelManager();
	void wlAddBuddy(const char* who, const char* channel);
	void wlSetAdapter(const char* adapter);
	void wlSetOptions(const char* options);
	char* wlListAdapters();
	char* wlListChannels();
	char* wlSelectedAdapter();
	char* wlSelectedChannel();
	void wlSendMessage(const char* who, const char* message);
	void wlGetMessageCallback(void (*func_ptr)(const char*, const char*));
#ifdef __cplusplus	
}
#endif
