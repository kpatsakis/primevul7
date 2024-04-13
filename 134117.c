static void pp_cleanup_pass(void)
{
    if (defining) {
        if (defining->name) {
            nasm_nonfatal("end of file while still defining macro `%s'",
                          defining->name);
        } else {
            nasm_nonfatal("end of file while still in %%rep");
        }

        free_mmacro(defining);
        defining = NULL;
    }

    while (cstk)
        ctx_pop();
    free_macros();
    while (istk) {
        Include *i = istk;
        istk = istk->next;
        fclose(i->fp);
        nasm_free(i);
    }
    while (cstk)
        ctx_pop();
    src_set_fname(NULL);
}