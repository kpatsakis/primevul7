STATIC regnode_offset /* Location. */
S_reganode(pTHX_ RExC_state_t *pRExC_state, U8 op, U32 arg)
{
    const regnode_offset ret = regnode_guts(pRExC_state, op, regarglen[op], "reganode");
    regnode_offset ptr = ret;

    PERL_ARGS_ASSERT_REGANODE;

    /* ANYOF are special cased to allow non-length 1 args */
    assert(regarglen[op] == 1);

    FILL_ADVANCE_NODE_ARG(ptr, op, arg);
    RExC_emit = ptr;
    return(ret);