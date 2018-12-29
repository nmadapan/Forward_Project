#ifndef __UTTERANCE_H__
#define __UTTERANCE_H__

#include <stdio.h>
#include <string.h>

#include "pocketsphinx.h"
#include "err.h"
#include "ad.h"
#include "cont_ad.h"

#include <signal.h>
#include <setjmp.h>



/* Sleep for specified msec */
static void
sleep_msec(int32 ms)
{
#if (defined(WIN32) && !defined(GNUWINCE)) || defined(_WIN32_WCE)
    Sleep(ms);
#else
    /* ------------------- Unix ------------------ */
    struct timeval tmo;

    tmo.tv_sec = 0;
    tmo.tv_usec = ms * 1000;

    select(0, NULL, NULL, NULL, &tmo);
#endif
}


static void
utterance_loop()
{
    int16 adbuf[4096];
    int32 k, ts, rem, score, prob;
    char const *hyp;
    char const *uttid;
    cont_ad_t *cont;
    char word[256];
	static ad_rec_t *ad;
	static ps_decoder_t *ps;

    /* Initialize continuous listening module */
    if ((cont = cont_ad_init(ad, ad_read)) == NULL)
        E_FATAL("cont_ad_init failed\n");
    if (ad_start_rec(ad) < 0)
        E_FATAL("ad_start_rec failed\n");
    if (cont_ad_calib(cont) < 0)
        E_FATAL("cont_ad_calib failed\n");

    for (;;) {
        /* Indicate listening for next utterance */
        printf("READY....\n");
        fflush(stdout);
        fflush(stderr);

        /* Await data for next utterance */
        while ((k = cont_ad_read(cont, adbuf, 4096)) == 0)
            sleep_msec(100);

        if (k < 0)
            E_FATAL("cont_ad_read failed\n");

        /*
         * Non-zero amount of data received; start recognition of new utterance.
         * NULL argument to uttproc_begin_utt => automatic generation of utterance-id.
         */
        if (ps_start_utt(ps, NULL) < 0)
            E_FATAL("ps_start_utt() failed\n");
        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
        printf("Listening...\n");
        fflush(stdout);

        /* Note timestamp for this first block of data */
        ts = cont->read_ts;

        /* Decode utterance until end (marked by a "long" silence, >1sec) */
        for (;;) {
            /* Read non-silence audio data, if any, from continuous listening module */
            if ((k = cont_ad_read(cont, adbuf, 4096)) < 0)
                E_FATAL("cont_ad_read failed\n");
            if (k == 0) {
                /*
                 * No speech data available; check current timestamp with most recent
                 * speech to see if more than 1 sec elapsed.  If so, end of utterance.
                 */
                if ((cont->read_ts - ts) > DEFAULT_SAMPLES_PER_SEC)
                    break;
            }
            else {
                /* New speech data received; note current timestamp */
                ts = cont->read_ts;
            }

            /*
             * Decode whatever data was read above.
             */
            rem = ps_process_raw(ps, adbuf, k, FALSE, FALSE);

            /* If no work to be done, sleep a bit */
            if ((rem == 0) && (k == 0))
                sleep_msec(20);
        }

        /*
         * Utterance ended; flush any accumulated, unprocessed A/D data and stop
         * listening until current utterance completely decoded
         */
        ad_stop_rec(ad);
        while (ad_read(ad, adbuf, 4096) >= 0);
        cont_ad_reset(cont);

        printf("Stopped listening, please wait...\n");
        fflush(stdout);
        /* Finish decoding, obtain and print result */
        ps_end_utt(ps);
        hyp = ps_get_hyp(ps, &score, &uttid);
		printf(" hyp:: %s: %s (%d)\n", uttid, hyp, score);
        fflush(stdout);


		/* Check Probability - Added by YuTing, Mar 5, 11
		refer by: https://sourceforge.net/projects/cmusphinx/forums/forum/5471/topic/3535201
		*/
		// get probability.  Value is on logarithmic scale
		prob = ps_get_prob(ps, &uttid);
		//printf("Probability = %d \n",prob);
		// convert the probability to a usable "percentage" value
		prob = 100*(logmath_exp(ps_get_logmath(ps), prob));
		// reject if the word is below the desired probability
		printf("Probability = %d \n",prob);
		if (prob < 50){
			printf("Rejection: do not pick up!");
		}

        /* Exit if the first word spoken was GOODBYE */
        if (hyp) {
            sscanf(hyp, "%s", word);
            if (strcmp(word, "goodbye") == 0)
                break;
        }

        /* Resume A/D recording for next utterance */
        if (ad_start_rec(ad) < 0)
            E_FATAL("ad_start_rec failed\n");
    }

    cont_ad_close(cont);
}

#endif
#endif