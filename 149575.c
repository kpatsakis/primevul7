rb_reg_fixed_encoding_p(VALUE re)
{
    if (FL_TEST(re, KCODE_FIXED))
        return Qtrue;
    else
        return Qfalse;
}