kcode_getter(void)
{
    rb_warn("variable $KCODE is no longer effective");
    return Qnil;
}