//author: Simon Kadel




#ifdef WHISPERSTATIC 
#	define WHISPERAPI
#else 
#	ifdef WHISPER_BUILD
#		define WHISPERAPI __declspec(dllexport)
#	else
#		define WHISPERAPI __declspec(dllimport)
#	endif
#endif
	
