rb_reg_named_captures(VALUE re)
{
    VALUE hash = rb_hash_new();
    rb_reg_check(re);
    onig_foreach_name(RREGEXP_PTR(re), reg_named_captures_iter, (void*)hash);
    return hash;
}