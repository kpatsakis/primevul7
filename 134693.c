gs_interp_make_oper(ref * opref, op_proc_t proc, int idx)
{
    int i;

    for (i = num_special_ops; i > 0 && proc != interp1_op_defs[i].proc; --i)
        DO_NOTHING;
    if (i > 0)
        make_tasv(opref, tx_op + (i - 1), a_executable, i, opproc, proc);
    else
        make_tasv(opref, t_operator, a_executable, idx, opproc, proc);
}