rb_reg_inspect(VALUE re)
{
    if (!RREGEXP_PTR(re) || !RREGEXP_SRC(re) || !RREGEXP_SRC_PTR(re)) {
        return rb_any_to_s(re);
    }
    return rb_reg_desc(RREGEXP_SRC_PTR(re), RREGEXP_SRC_LEN(re), re);
}