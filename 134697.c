gs_interp_init(i_ctx_t **pi_ctx_p, const ref *psystem_dict,
               gs_dual_memory_t *dmem)
{
    /* Create and initialize a context state. */
    gs_context_state_t *pcst = 0;
    int code = context_state_alloc(&pcst, psystem_dict, dmem);
    if (code >= 0) {
        code = context_state_load(pcst);
        if (code < 0) {
            context_state_free(pcst);
            pcst = NULL;
        }
    }

    if (code < 0)
        lprintf1("Fatal error %d in gs_interp_init!\n", code);
    *pi_ctx_p = pcst;

    return code;
}