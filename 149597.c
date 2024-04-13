rb_reg_source(VALUE re)
{
    VALUE str;

    rb_reg_check(re);
    str = rb_str_dup(RREGEXP_SRC(re));
    if (OBJ_TAINTED(re)) OBJ_TAINT(str);
    return str;
}