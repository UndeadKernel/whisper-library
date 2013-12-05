//author: Simon Kadel

#ifndef WIN32 
#	define WHISPERAPI
#else 
#	ifdef WHISPER_BUILD
#		define WHISPERAPI __declspec(dllexport)
#	else
#		define WHISPERAPI __declspec(dllimport)
#	endif
#endif
	
