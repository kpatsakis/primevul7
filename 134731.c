do_call_operator(op_proc_t op_proc, i_ctx_t *i_ctx_p)
{
    int code;
    code = op_proc(i_ctx_p);
    if (gs_debug_c(gs_debug_flag_validate_clumps))
        ivalidate_clean_spaces(i_ctx_p);
    return code; /* A good place for a conditional breakpoint. */
}