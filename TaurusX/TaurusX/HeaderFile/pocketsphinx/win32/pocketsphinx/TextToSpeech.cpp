#include <sapi.h>

void TextToSpeech(hpy){
	ISpVoice * pVoice = NULL;
	HRESULT hr;

    if (FAILED(::CoInitialize(NULL)))
        return FALSE;

    hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
    if( SUCCEEDED( hr ) )
    {
        hr = pVoice->Speak(L"Oh oh oh oh my god", 0, NULL);
        pVoice->Release();
        pVoice = NULL;
    }
    ::CoUninitialize();
    //return TRUE;
}