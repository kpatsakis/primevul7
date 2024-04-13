op_is_change(int op)
{
    return opchars[op][2] & OPF_CHANGE;
}