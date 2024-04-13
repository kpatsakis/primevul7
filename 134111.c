pp_reset(const char *file, enum preproc_mode mode, struct strlist *dep_list)
{
    int apass;
    struct Include *inc;

    cstk = NULL;
    defining = NULL;
    nested_mac_count = 0;
    nested_rep_count = 0;
    init_macros();
    unique = 0;
    deplist = dep_list;
    pp_mode = mode;

    /* Reset options to default */
    nasm_zero(ppopt);

    if (!use_loaded)
        use_loaded = nasm_malloc(use_package_count * sizeof(bool));
    memset(use_loaded, 0, use_package_count * sizeof(bool));

    /* First set up the top level input file */
    nasm_new(istk);
    istk->fp = nasm_open_read(file, NF_TEXT);
    if (!istk->fp) {
	nasm_fatalf(ERR_NOFILE, "unable to open input file `%s'%s%s",
                    file, errno ? " " : "", errno ? strerror(errno) : "");
    }
    src_set(0, file);
    istk->where = src_where();
    istk->lineinc = 1;

    strlist_add(deplist, file);

    /*
     * Set up the stdmac packages as a virtual include file,
     * indicated by a null file pointer.
     */
    nasm_new(inc);
    inc->next = istk;
    src_set(0, NULL);
    inc->where = src_where();
    inc->nolist = !list_option('b');
    istk = inc;
    lfmt->uplevel(LIST_INCLUDE, 0);

    pp_add_magic_stdmac();

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
     * Define the __?PASS?__ macro.  This is defined here unlike all the
     * other builtins, because it is special -- it varies between
     * passes -- but there is really no particular reason to make it
     * magic.
     *
     * 0 = dependencies only
     * 1 = preparatory passes
     * 2 = final pass
     * 3 = preproces only
     */
    switch (mode) {
    case PP_NORMAL:
        apass = pass_final() ? 2 : 1;
        break;
    case PP_DEPS:
        apass = 0;
        break;
    case PP_PREPROC:
        apass = 3;
        break;
    default:
        panic();
    }

    define_smacro("__?PASS?__", true, make_tok_num(NULL, apass), NULL);
}