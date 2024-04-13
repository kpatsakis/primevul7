STATIC regnode_offset
S_reg2Lanode(pTHX_ RExC_state_t *pRExC_state, const U8 op, const U32 arg1, const I32 arg2)
{
    /* emit a node with U32 and I32 arguments */

    const regnode_offset ret = regnode_guts(pRExC_state, op, regarglen[op], "reg2Lanode");
    regnode_offset ptr = ret;

    PERL_ARGS_ASSERT_REG2LANODE;

    assert(regarglen[op] == 2);

    FILL_ADVANCE_NODE_2L_ARG(ptr, op, arg1, arg2);
    RExC_emit = ptr;
    return(ret);