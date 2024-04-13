update_char_offset(VALUE match)
{
    struct rmatch *rm = RMATCH(match)->rmatch;
    struct re_registers *regs;
    int i, num_regs, num_pos;
    long c;
    char *s, *p, *q;
    rb_encoding *enc;
    pair_t *pairs;

    if (rm->char_offset_updated)
        return;

    regs = &rm->regs;
    num_regs = rm->regs.num_regs;

    if (rm->char_offset_num_allocated < num_regs) {
        REALLOC_N(rm->char_offset, struct rmatch_offset, num_regs);
        rm->char_offset_num_allocated = num_regs;
    }

    enc = rb_enc_get(RMATCH(match)->str);
    if (rb_enc_mbmaxlen(enc) == 1) {
        for (i = 0; i < num_regs; i++) {
            rm->char_offset[i].beg = BEG(i);
            rm->char_offset[i].end = END(i);
        }
        rm->char_offset_updated = 1;
        return;
    }

    pairs = ALLOCA_N(pair_t, num_regs*2);
    num_pos = 0;
    for (i = 0; i < num_regs; i++) {
        if (BEG(i) < 0)
            continue;
        pairs[num_pos++].byte_pos = BEG(i);
        pairs[num_pos++].byte_pos = END(i);
    }
    qsort(pairs, num_pos, sizeof(pair_t), pair_byte_cmp);

    s = p = RSTRING_PTR(RMATCH(match)->str);
    c = 0;
    for (i = 0; i < num_pos; i++) {
        q = s + pairs[i].byte_pos;
        c += rb_enc_strlen(p, q, enc);
        pairs[i].char_pos = c;
        p = q;
    }

    for (i = 0; i < num_regs; i++) {
        pair_t key, *found;
        if (BEG(i) < 0) {
            rm->char_offset[i].beg = -1;
            rm->char_offset[i].end = -1;
            continue;
        }

        key.byte_pos = BEG(i);
        found = bsearch(&key, pairs, num_pos, sizeof(pair_t), pair_byte_cmp);
        rm->char_offset[i].beg = found->char_pos;

        key.byte_pos = END(i);
        found = bsearch(&key, pairs, num_pos, sizeof(pair_t), pair_byte_cmp);
        rm->char_offset[i].end = found->char_pos;
    }

    rm->char_offset_updated = 1;
}