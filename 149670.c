match_hash(VALUE match)
{
    const struct re_registers *regs;
    st_index_t hashval;

    match_check(match);
    hashval = rb_hash_start(rb_str_hash(RMATCH(match)->str));
    hashval = rb_hash_uint(hashval, reg_hash(RMATCH(match)->regexp));
    regs = RMATCH_REGS(match);
    hashval = rb_hash_uint(hashval, regs->num_regs);
    hashval = rb_hash_uint(hashval, rb_memhash(regs->beg, regs->num_regs * sizeof(*regs->beg)));
    hashval = rb_hash_uint(hashval, rb_memhash(regs->end, regs->num_regs * sizeof(*regs->end)));
    hashval = rb_hash_end(hashval);
    return ST2FIX(hashval);
}