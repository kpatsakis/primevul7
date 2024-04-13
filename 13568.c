nfa_re_num_cmp(long_u val, int op, long_u pos)
{
    if (op == 1) return pos > val;
    if (op == 2) return pos < val;
    return val == pos;
}