// (C) Mithun Jacob
#ifndef __TEXT2SPEECH_H__
#define __TEXT2SPEECH_H__

#include <sapi.h>
using namespace std;

class SAM
{
private:
	ISpVoice * pVoice;
	HRESULT hr;
	// string to wstring
	std::wstring s2ws(const std::string & s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}
public:
	// Constructor
	enum mode{SYNC = SPF_DEFAULT,ASYNC = SPF_ASYNC};
	SAM():pVoice(NULL)
	{
		init();
	}
	~SAM()
	{
		pVoice->Release();
		pVoice = NULL;
		::CoUninitialize();
	}
	// Initalize SAM
	bool init()
	{
		if (FAILED(::CoInitialize(NULL))) return false;
		hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
		return true;
	}
	// Speak
	void setVolRate(const int vol = 100, const int rate = 0)
	{
		pVoice->SetRate(rate);
		pVoice->SetVolume(vol);
	}
	bool speak(string text, const mode flags = ASYNC)
	{		
		if( SUCCEEDED( hr ) )
		{			

			hr = pVoice->Speak(s2ws(text).c_str(),flags, NULL);
			return true;
		}
		else return false;
	}
};

#endif