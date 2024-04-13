rb_reg_init_copy(VALUE copy, VALUE re)
{
    if (!OBJ_INIT_COPY(copy, re)) return copy;
    rb_reg_check(re);
    return rb_reg_init_str(copy, RREGEXP_SRC(re), rb_reg_options(re));
}