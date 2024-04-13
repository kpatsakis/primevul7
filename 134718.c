zactonuel(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    push(1);
    make_bool(op, !!gs_lib_ctx_get_act_on_uel((gs_memory_t *)(i_ctx_p->memory.current)));
    return 0;
}