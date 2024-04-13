ignorecase_getter(void)
{
    rb_warn("variable $= is no longer effective");
    return Qfalse;
}