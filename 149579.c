rb_reg_options(VALUE re)
{
    int options;

    rb_reg_check(re);
    options = RREGEXP_PTR(re)->options & ARG_REG_OPTION_MASK;
    if (RBASIC(re)->flags & KCODE_FIXED) options |= ARG_ENCODING_FIXED;
    if (RBASIC(re)->flags & REG_ENCODING_NONE) options |= ARG_ENCODING_NONE;
    return options;
}