// (C) Mithun Jacob
#ifndef __SPHINX_H__
#define __SPHINX_H__

#include <stdio.h>
#include <string.h>
extern "C"
	{
	#include "pocketsphinx.h"
	#include "err.h"
	#include "ad.h"
	#include "cont_ad.h"
	}
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include <string>
#include <conio.h>
#include <opencv2/highgui/highgui.hpp>

// Graceful exit
#define GEXIT(msg) {printf(msg);return false;}

using namespace std;
using namespace cv;

namespace SPHINX
{
	// Variables
	cmd_ln_t *config;
	static ad_rec_t *ad;
	static ps_decoder_t *ps;
	static jmp_buf jbuf;
	// Continuous variables
	int16 adbuf[4096];
	int32 k, ts, rem, score;
	char const *hyp;
	char const *uttid;
	cont_ad_t *cont;
	// Functions
	static void sighandler(int signo)
	{
		longjmp(SPHINX::jbuf, 1);
	}
	/* Sleep for specified msec */
	static void sleep_msec(int32 ms)
	{
		Sleep(ms);
	}
	////////////////////////////////////////////////'STRUCTORS////////////////////////////////////////////
	bool init(const string HMM, const string LM, const string DICT)
	{
		printf("\nInitializing Sphinx ... ");
		config = cmd_ln_init(NULL,ps_args(),TRUE,"-hmm",HMM.c_str(),"-lm",LM.c_str(),"-dict",DICT.c_str(),NULL, "-adcdev","sysdefault");
		signal(SIGINT, &sighandler);
		if(config == NULL) return false;
		ps = ps_init(config);
		printf("\nInitializing PS ... ");
		if(ps == NULL) return false;
		if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"), (int)cmd_ln_float32_r(config, "-samprate"))) == NULL) GEXIT("ad_open_dev failed\n");
		/* Initialize continuous listening module */
		if ((cont = cont_ad_init(ad, ad_read)) == NULL) GEXIT("cont_ad_init failed\n");
		if (ad_start_rec(ad) < 0) GEXIT("ad_start_rec failed\n");
		if (cont_ad_calib(cont) < 0) GEXIT("cont_ad_calib failed\n");
		return true;
	}
	void release()
	{
		cont_ad_close(cont);
		ps_free(ps);
		ad_close(ad);
	}
	///////////////////////////////////////////POLLING LOOP///////////////////////////////////
	string hypothesis;
	double probability;
	bool utterance(int* start )
	{
		/* Indicate listening for next utterance */
		printf("READY....\n");
		fflush(stdout);
		fflush(stderr);

		/* Await data for next utterance */
		while ((k = cont_ad_read(cont, adbuf, 4096)) == 0) {if(*start == 0 || waitKey(30) == 27) break;}

		if (k < 0) E_FATAL("cont_ad_read failed\n");
		/*
		 * Non-zero amount of data received; start recognition of new utterance.
		 * NULL argument to uttproc_begin_utt => automatic generation of utterance-id.
		 */
		if (ps_start_utt(ps, NULL) < 0)	E_FATAL("ps_start_utt() failed\n");
		ps_process_raw(ps, adbuf, k, FALSE, FALSE);
		printf("Listening...\n");
        
		fflush(stdout);
		
		/* Note timestamp for this first block of data */
		ts = cont->read_ts;
		
		/* Decode utterance until end (marked by a "long" silence, >1sec) */
		for (;;) {			
			if(cv::waitKey(30)==27) return false;
			/* Read non-silence audio data, if any, from continuous listening module */
			if ((k = cont_ad_read(cont, adbuf, 4096)) < 0)	E_FATAL("cont_ad_read failed\n");
				/*
				 * No speech data available; check current timestamp with most recent
				 * speech to see if more than 1 sec elapsed.  If so, end of utterance.
				 */
			if (k == 0) 
			{
				if ((cont->read_ts - ts) > DEFAULT_SAMPLES_PER_SEC) break;
			}
			/* New speech data received; note current timestamp */
			else ts = cont->read_ts;
			// Decode whatever data was read above.
			rem = ps_process_raw(ps, adbuf, k, FALSE, FALSE);
			/* If no work to be done, sleep a bit */
			if ((rem == 0) && (k == 0))	sleep_msec(20);
		}

		// Utterance ended; flush any accumulated, 
		ad_stop_rec(ad);
		while (ad_read(ad, adbuf, 4096) >= 0);
		cont_ad_reset(cont);

		//printf("Stopped listening, please wait...\n");
		fflush(stdout);
		/* Finish decoding, obtain and print result */
		ps_end_utt(ps);
		hyp = ps_get_hyp(ps, &score, &uttid);
		fflush(stdout);
		// get probability.  Value is on logarithmic scale
		int ps_prob = ps_get_prob(ps, &uttid);
		// convert the probability to a usable "percentage" value
		if(hyp == NULL) 
		{
			hypothesis = "";
			probability = 0;
		}
		else
		{
			probability = 100*(logmath_exp(ps_get_logmath(ps), ps_prob));
			hypothesis = string(hyp);
		}
		/* Resume A/D recording for next utterance */
		if (ad_start_rec(ad) < 0) E_FATAL("ad_start_rec failed\n");
		return true;
	}
};
#endif