rb_reg_hash(VALUE re)
{
    st_index_t hashval = reg_hash(re);
    return ST2FIX(hashval);
}