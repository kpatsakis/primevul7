rb_reg_s_alloc(VALUE klass)
{
    NEWOBJ_OF(re, struct RRegexp, klass, T_REGEXP | (RGENGC_WB_PROTECTED_REGEXP ? FL_WB_PROTECTED : 0));

    re->ptr = 0;
    RB_OBJ_WRITE(re, &re->src, 0);
    re->usecnt = 0;

    return (VALUE)re;
}