STATIC regnode_offset /* Location. */
S_reg_node(pTHX_ RExC_state_t *pRExC_state, U8 op)
{
    const regnode_offset ret = regnode_guts(pRExC_state, op, regarglen[op], "reg_node");
    regnode_offset ptr = ret;

    PERL_ARGS_ASSERT_REG_NODE;

    assert(regarglen[op] == 0);

    FILL_ADVANCE_NODE(ptr, op);
    RExC_emit = ptr;
    return(ret);