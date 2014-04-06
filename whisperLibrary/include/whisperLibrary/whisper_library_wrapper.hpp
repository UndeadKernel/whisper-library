#ifdef __cplusplus
extern "C" {
#endif
	#include <stdbool.h>

	typedef struct AdapterList {
		char* name;
		char* description;
		struct AdapterList* next;
	} AdapterList;

	typedef struct ChannelList {
		char* id;
		char* name;
		char* info;
		struct ChannelList* next;
	} ChannelList;

	void wlMakeChannelManager();
	void wlDestroyChannelManager();
	ChannelList* wlListChannels();
	unsigned int wChannelCount();
	void wlSetOptions(const char* ip, const char* options);
	bool wlOpenConnection(const char* ip, const char* channel_id);
	void wlCloseConnection(const char* ip);
	unsigned int wlConnectionCount();
	void wlSendMessage(const char* who, const char* message);
	bool wlSetAdapter(const char* adapter);
	unsigned int wlAdapterCount();
	AdapterList* wlListAdapters();
	char* wlFindAdapter();
	void wlSetMessageCallback(void (*func_ptr)(const char*, const char*));
#ifdef __cplusplus	
}
#endif
