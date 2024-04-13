do_call_operator_verbose(op_proc_t op_proc, i_ctx_t *i_ctx_p)
{
    int code;

#ifndef SHOW_STACK_DEPTHS
    if_debug1m('!', imemory, "[!]operator %s\n", op_get_name_string(op_proc));
#else
    if_debug3m('!', imemory, "[!][es=%d os=%d]operator %s\n",
            esp-i_ctx_p->exec_stack.stack.bot,
            osp-i_ctx_p->op_stack.stack.bot,
            op_get_name_string(op_proc));
#endif
    code = do_call_operator(op_proc, i_ctx_p);
#if defined(SHOW_STACK_DEPTHS)
    if_debug2m('!', imemory, "[!][es=%d os=%d]\n",
            esp-i_ctx_p->exec_stack.stack.bot,
            osp-i_ctx_p->op_stack.stack.bot);
#endif
    if (gs_debug_c(gs_debug_flag_validate_clumps))
        ivalidate_clean_spaces(i_ctx_p);
    return code; /* A good place for a conditional breakpoint. */
}