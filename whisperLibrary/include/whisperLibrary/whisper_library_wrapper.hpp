
#ifdef __cplusplus
extern "C" {
#endif
	struct WhisperMessage{
		char* who;
		char* message;
	};
	void wlMakeChannelManager();
	void wlDestroyChannelManager();
	const char* wlListChannels();
	unsigned int wChannelCount();
	void wlSetOptions(const char* ip, const char* options);
	bool wlOpenConnection(const char* ip, unsigned int channel_id);
	void wlCloseConnection(const char* ip);
	unsigned int wlConnectionCount();
	void wlSendMessage(const char* who, const char* message);
	void wlSetAdapter(const char* adapter);
	unsigned int wlAdapterCount();
	const char* wlListAdapters();
	void wlSetMessageCallback(void (*func_ptr)(const char*, const char*));
#ifdef __cplusplus	
}
#endif
