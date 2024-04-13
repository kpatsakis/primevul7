match_named_captures_iter(const OnigUChar *name, const OnigUChar *name_end,
	int back_num, int *back_refs, OnigRegex regex, void *arg) {
    struct MEMO *memo = MEMO_CAST(arg);
    VALUE hash = memo->v1;
    VALUE match = memo->v2;

    VALUE key = rb_enc_str_new((const char *)name, name_end-name, regex->enc);
    VALUE value;

    int i;
    int found = 0;

    for (i = 0; i < back_num; i++) {
	value = rb_reg_nth_match(back_refs[i], match);
	if (RTEST(value)) {
	    rb_hash_aset(hash, key, value);
	    found = 1;
	}
    }

    if (found == 0) {
	rb_hash_aset(hash, key, Qnil);
    }

    return 0;
}