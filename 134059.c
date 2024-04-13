static char *line_from_stdmac(void)
{
    unsigned char c;
    const unsigned char *p = stdmacpos;
    char *line, *q;
    size_t len = 0;

    if (!stdmacpos)
        return NULL;

    /*
     * 32-126 is ASCII, 127 is end of line, 128-31 are directives
     * (allowed to wrap around) corresponding to PP_* tokens 0-159.
     */
    while ((c = *p++) != 127) {
        uint8_t ndir = c - 128;
        if (ndir < 256-96)
            len += pp_directives_len[ndir] + 1;
        else
            len++;
    }

    line = nasm_malloc(len + 1);
    q = line;

    while ((c = *stdmacpos++) != 127) {
        uint8_t ndir = c - 128;
        if (ndir < 256-96) {
            memcpy(q, pp_directives[ndir], pp_directives_len[ndir]);
            q += pp_directives_len[ndir];
            *q++ = ' ';
        } else {
            *q++ = c;
        }
    }
    stdmacpos = p;
    *q = '\0';

    if (*stdmacpos == 127) {
        /* This was the last of this particular macro set */
        stdmacpos = NULL;
        if (*stdmacnext) {
            stdmacpos = *stdmacnext++;
        } else if (do_predef) {
            Line *pd, *l;

            /*
             * Nasty hack: here we push the contents of
             * `predef' on to the top-level expansion stack,
             * since this is the most convenient way to
             * implement the pre-include and pre-define
             * features.
             */
            list_for_each(pd, predef) {
                nasm_new(l);
                l->next     = istk->expansion;
                l->first    = dup_tlist(pd->first, NULL);
                l->finishes = NULL;

                istk->expansion = l;
            }
            do_predef = false;
        }
    }

    return line;
}