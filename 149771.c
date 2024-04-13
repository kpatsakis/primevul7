rb_reg_casefold_p(VALUE re)
{
    rb_reg_check(re);
    if (RREGEXP_PTR(re)->options & ONIG_OPTION_IGNORECASE) return Qtrue;
    return Qfalse;
}