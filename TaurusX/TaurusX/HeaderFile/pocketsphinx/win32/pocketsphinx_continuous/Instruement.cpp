#include <pocketsphinx.h>

#define MODELDIR "C:\\Documents and Settings\\YuTing Li\\My Documents\\2011 Spring\\2011 IROS Speech\\pocketsphinx\\model"
int
main(int argc, char *argv[])
{
        ps_decoder_t *ps;
        cmd_ln_t *config;

        config = cmd_ln_init(NULL, ps_args(), TRUE,
                             "-hmm", MODELDIR "C:\\Documents and Settings\\YuTing Li\\My Documents\\2011 Spring\\2011 IROS Speech\\pocketsphinx\\model\\hmm\\en_US\\hub4wsj_sc_8k",
                             "-lm", MODELDIR "C:\\DIC\\9835.lm",
                             "-dict", MODELDIR "C:\\DIC\\9835.dic",
                             NULL);
		ps = ps_init(config);
        if (ps == NULL)
                return 1;
        if (config == NULL)
                return 1;

        return 0;
}