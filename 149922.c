reg_hash(VALUE re)
{
    st_index_t hashval;

    rb_reg_check(re);
    hashval = RREGEXP_PTR(re)->options;
    hashval = rb_hash_uint(hashval, rb_memhash(RREGEXP_SRC_PTR(re), RREGEXP_SRC_LEN(re)));
    return rb_hash_end(hashval);
}