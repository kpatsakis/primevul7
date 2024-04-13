rb_reg_names(VALUE re)
{
    VALUE ary;
    rb_reg_check(re);
    ary = rb_ary_new_capa(onig_number_of_names(RREGEXP_PTR(re)));
    onig_foreach_name(RREGEXP_PTR(re), reg_names_iter, (void*)ary);
    return ary;
}