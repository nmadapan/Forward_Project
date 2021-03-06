
#include <stdio.h>
#include <string.h>

#include "pocketsphinx.h"
#include "err.h"
#include "ad.h"
#include "cont_ad.h"
#include <signal.h>
#include <setjmp.h>
#include <time.h>

#define MODELDIR "C:/Documents and Settings/YuTing Li/My Documents/2011 Spring/2011 IROS Speech/pocketsphinx/model"

static const arg_t cont_args_def[] = {
    POCKETSPHINX_OPTIONS,
    /* Argument file. */
    { "-argfile",
      ARG_STRING,
      NULL,
      "Argument file giving extra arguments." },
    { "-adcdev", ARG_STRING, NULL, "Name of audio device to use for input." },
    CMDLN_EMPTY_OPTION
};


static ad_rec_t *ad;
static ps_decoder_t *ps;
static jmp_buf jbuf;


/* Sleep for specified msec */
static void sleep_msec(int32 ms)
{
    Sleep(ms);
}



static void utterance_loop(int32 *prob, char const *word, int pickup)
{
    int16 adbuf[4096];
    int32 k,i, ts, rem, score;
    char const *hyp;
    char const *uttid;
    cont_ad_t *cont;
    //char word[256];
	char instrument[5][10] = {"SCALPEL", "SCISSORS", "RETRACTOR", "FORCEPS", "HEMOSTAT" };

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
		//start = clock();	
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
            // Decode whatever data was read above.
            rem = ps_process_raw(ps, adbuf, k, FALSE, FALSE);
            /* If no work to be done, sleep a bit */
            if ((rem == 0) && (k == 0))
                sleep_msec(20);
        }

        // Utterance ended; flush any accumulated, 
        ad_stop_rec(ad);
        while (ad_read(ad, adbuf, 4096) >= 0);
        cont_ad_reset(cont);

        printf("Stopped listening, please wait...\n");
		//end = clock();
        fflush(stdout);
        /* Finish decoding, obtain and print result */
        ps_end_utt(ps);
        hyp = ps_get_hyp(ps, &score, &uttid);
		
		//*dif = (end-start) / 100;
		printf(" hyp:: %s: %s (%d) \n", uttid, hyp, score);
        fflush(stdout);
        sscanf(hyp, "%s", word);
        if (hyp) {
            if (strcmp(word, "GOODBYE") == 0)
                break;
        }
		pickup = 1;
		/* Check Probability - Added @ Mar 5, 11 */
		// get probability.  Value is on logarithmic scale
		*prob = ps_get_prob(ps, &uttid);
		// convert the probability to a usable "percentage" value
		*prob = 100*(logmath_exp(ps_get_logmath(ps), *prob));
		// reject if the word is below the desired probability
		printf("Probability = %d \n",*prob);
		if (*prob < 80){
			pickup = 0;
			strcpy(word,"EMPTY");
			printf("Rejection: do not pick up! \n");
		}
        /* Exit if the word spoken was GOODBYE, then exit */

		
        /* Resume A/D recording for next utterance */
        if (ad_start_rec(ad) < 0)
            E_FATAL("ad_start_rec failed\n");
    }
    cont_ad_close(cont);
}

static void sighandler(int signo)
{
    longjmp(jbuf, 1);
}

int main(int argc, char *argv[])
{
    cmd_ln_t *config;
    char const *cfg;
	int32 *prob;
	char const *name;
	int *pickup;
	// initialization: create a configuration object 
	config = cmd_ln_init(NULL, ps_args(), TRUE,
			     "-hmm", MODELDIR "/hmm/en_US/hub4wsj_sc_8k",
			     "-lm", MODELDIR "/lm/en/turtle.DMP",
			     "-dict", MODELDIR "/lm/en/turtle.dic",
			     NULL);

    signal(SIGINT, &sighandler);
	config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
    if (config == NULL)
        return 1;
    ps = ps_init(config);
    if (ps == NULL)
        return 1;
    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"), (int)cmd_ln_float32_r(config, "-samprate"))) == NULL)
        E_FATAL("ad_open_dev failed\n");

    E_INFO("%s COMPILED ON: %s, AT: %s\n\n", argv[0], __DATE__, __TIME__);
	

    if (setjmp(jbuf) == 0) {
        utterance_loop(&prob, &name, &pickup);
    }

    ps_free(ps);
    ad_close(ad);
    return 0;
}

