pp_reset(char *file, int apass, StrList **deplist)
{
    Token *t;

    cstk = NULL;
    istk = nasm_malloc(sizeof(Include));
    istk->next = NULL;
    istk->conds = NULL;
    istk->expansion = NULL;
    istk->mstk = NULL;
    istk->fp = nasm_open_read(file, NF_TEXT);
    istk->fname = NULL;
    src_set(0, file);
    istk->lineinc = 1;
    if (!istk->fp)
	nasm_fatal(ERR_NOFILE, "unable to open input file `%s'", file);
    defining = NULL;
    nested_mac_count = 0;
    nested_rep_count = 0;
    init_macros();
    unique = 0;

    if (tasm_compatible_mode)
        pp_add_stdmac(nasm_stdmac_tasm);

    pp_add_stdmac(nasm_stdmac_nasm);
    pp_add_stdmac(nasm_stdmac_version);

    if (extrastdmac)
        pp_add_stdmac(extrastdmac);

    stdmacpos  = stdmacros[0];
    stdmacnext = &stdmacros[1];

    do_predef = true;

    /*
     * 0 for dependencies, 1 for preparatory passes, 2 for final pass.
     * The caller, however, will also pass in 3 for preprocess-only so
     * we can set __PASS__ accordingly.
     */
    pass = apass > 2 ? 2 : apass;

    dephead = deplist;
    nasm_add_string_to_strlist(dephead, file);
    
    /*
     * Define the __PASS__ macro.  This is defined here unlike
     * all the other builtins, because it is special -- it varies between
     * passes.
     */
    t = nasm_malloc(sizeof(*t));
    t->next = NULL;
    make_tok_num(t, apass);
    t->a.mac = NULL;
    define_smacro(NULL, "__PASS__", true, 0, t);
}