
#include <stdio.h>
#include <string.h>

#include "pocketsphinx.h"
#include "err.h"
#include "ad.h"
#include "cont_ad.h"
#include "utterance.h"

#if !defined(_WIN32_WCE)
#include <signal.h>
#include <setjmp.h>

#endif
#if defined(WIN32) && !defined(GNUWINCE)
#include <time.h>

#else
#include <sys/types.h>
#include <sys/time.h>
#endif

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

static void sighandler(int signo)
{
    longjmp(jbuf, 1);
}

int main(int argc, char *argv[])
{
    cmd_ln_t *config;
    char const *cfg;

	config = cmd_ln_init(NULL, ps_args(), TRUE,
			     "-mdef", MODELDIR "/hmm/en_US/hub4wsj_sc_8k",
			     "-lm", MODELDIR "/lm/en/turtle.DMP",
			     "-dict", MODELDIR "/lm/en/turtle.dic",
			     NULL);

    /* Make sure we exit cleanly (needed for profiling among other things) */
    /* Signals seem to be broken in arm-wince-pe. */
#if !defined(GNUWINCE) && !defined(_WIN32_WCE)
    signal(SIGINT, &sighandler);
#endif

    if (argc == 2) {
        config = cmd_ln_parse_file_r(NULL, cont_args_def, argv[1], TRUE);
    }
    else {
        config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
    }
    /* Handle argument file as -argfile. */
    if (config && (cfg = cmd_ln_str_r(config, "-argfile")) != NULL) {
        config = cmd_ln_parse_file_r(config, cont_args_def, cfg, FALSE);
    }
    if (config == NULL)
        return 1;
    ps = ps_init(config);
    if (ps == NULL)
        return 1;

    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
                          (int)cmd_ln_float32_r(config, "-samprate"))) == NULL)
        E_FATAL("ad_open_dev failed\n");

    E_INFO("%s COMPILED ON: %s, AT: %s\n\n", argv[0], __DATE__, __TIME__);

    if (setjmp(jbuf) == 0) {
        utterance_loop();
    }

    ps_free(ps);
    ad_close(ad);

    return 0;
}

/** Silvio Moioli: Windows CE/Mobile entry point added. */
#if defined(_WIN32_WCE)
#pragma comment(linker,"/entry:mainWCRTStartup")
#include <windows.h>

//Windows Mobile has the Unicode main only
int wmain(int32 argc, wchar_t *wargv[]) {
    char** argv;
    size_t wlen;
    size_t len;
    int i;

    argv = malloc(argc*sizeof(char*));
    for (i=0; i<argc; i++){
        wlen = lstrlenW(wargv[i]);
        len = wcstombs(NULL, wargv[i], wlen);
        argv[i] = malloc(len+1);
        wcstombs(argv[i], wargv[i], wlen);
    }

    //assuming ASCII parameters
    return main(argc, argv);
}
#endif
