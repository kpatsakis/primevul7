rb_reg_new_str(VALUE s, int options)
{
    return rb_reg_init_str(rb_reg_alloc(), s, options);
}