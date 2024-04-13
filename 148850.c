ast_for_listcomp(struct compiling *c, const node *n)
{
    assert(TYPE(n) == (testlist_comp));
    return ast_for_itercomp(c, n, COMP_LISTCOMP);
}