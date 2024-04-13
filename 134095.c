static void do_clear(enum clear_what what, bool context)
{
    if (context) {
        if (what & CLEAR_ALLDEFINE) {
            Context *ctx;
            list_for_each(ctx, cstk)
                clear_smacro_table(&ctx->localmac, what);
        }
        /* Nothing else can be context-local */
    } else {
        if (what & CLEAR_ALLDEFINE)
            clear_smacro_table(&smacros, what);
        if (what & CLEAR_MMACRO)
            free_mmacro_table(&mmacros);
    }
}