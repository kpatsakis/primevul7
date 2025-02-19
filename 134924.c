static void
S_free_codeblocks(pTHX_ struct reg_code_blocks *cbs)
{
    int n;

    if (--cbs->refcnt > 0)
        return;
    for (n = 0; n < cbs->count; n++) {
        REGEXP *rx = cbs->cb[n].src_regex;
        if (rx) {
            cbs->cb[n].src_regex = NULL;
            SvREFCNT_dec_NN(rx);
        }
    }
    Safefree(cbs->cb);
    Safefree(cbs);