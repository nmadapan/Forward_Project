#include "Sphinx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include "Text2Speech.h"
using namespace std;




int main(int argc, char *argv[])
{

	// TSP parameters
	SAM sam;
	
	int *bStart;
	bStart = new int;
	*bStart = 1;
	double threshold = 80;
	bool initialization;
	const string HMM = "E:\\SpeechTSP\\HeaderFile\\pocketsphinx\\model\\hmm\\en_US\\hub4wsj_sc_8k";
	const string LM = "E:\\SpeechTSP\\HeaderFile\\7952.lm";
	const string DICT = "E:\\SpeechTSP\\HeaderFile\\7952.dic";

	//////////// Speeach recognition
	string S;
	initialization = SPHINX::init(HMM.c_str(), LM.c_str(), DICT.c_str());

	while(1)
	{
		
		if(SPHINX::utterance(bStart) == false) break;
		if(SPHINX::probability > threshold){ 
			S = SPHINX::hypothesis;
			cout<<"Speech: "<<S<<", condifence: "<<SPHINX::probability<<endl;
			// Matching to directions
			if (S.compare("YES") == 0){
				cout<<"YES@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
			}
		}
				
	}
	SPHINX::release();
	printf("Exit::Speech\n");
	return 0;
}