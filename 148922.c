FstringParser_check_invariants(FstringParser *state)
{
    if (state->last_str)
        assert(PyUnicode_CheckExact(state->last_str));
    ExprList_check_invariants(&state->expr_list);
}