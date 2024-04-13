zforceinterp_exit(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    stream *s;

    check_file(s, op);
    i_ctx_p->uel_position = stell(s)-1;
    /* resetfile */
    if (file_is_valid(s, op))
        sreset(s);

    if (!gs_lib_ctx_get_act_on_uel((gs_memory_t *)(i_ctx_p->memory.current)))
        return 0;

    gs_interp_reset(i_ctx_p);
    /* gs_interp_reset() actually leaves the op stack one entry below
     * the bottom of the stack, and that can cause problems depending
     * on the interpreter state at the end of the job.
     * So push a null object, and the return code before continuing.
     */
    push(2);
    op = osp;
    make_null(op - 1);
    make_int(op, gs_error_InterpreterExit);
    return_error(gs_error_Quit);
}