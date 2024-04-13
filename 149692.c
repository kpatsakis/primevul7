rb_reg_to_s(VALUE re)
{
    int options, opt;
    const int embeddable = ONIG_OPTION_MULTILINE|ONIG_OPTION_IGNORECASE|ONIG_OPTION_EXTEND;
    long len;
    const UChar* ptr;
    VALUE str = rb_str_buf_new2("(?");
    char optbuf[5];
    rb_encoding *enc = rb_enc_get(re);

    rb_reg_check(re);

    rb_enc_copy(str, re);
    options = RREGEXP_PTR(re)->options;
    ptr = (UChar*)RREGEXP_SRC_PTR(re);
    len = RREGEXP_SRC_LEN(re);
  again:
    if (len >= 4 && ptr[0] == '(' && ptr[1] == '?') {
	int err = 1;
	ptr += 2;
	if ((len -= 2) > 0) {
	    do {
                opt = char_to_option((int )*ptr);
                if (opt != 0) {
                    options |= opt;
                }
                else {
                    break;
                }
		++ptr;
	    } while (--len > 0);
	}
	if (len > 1 && *ptr == '-') {
	    ++ptr;
	    --len;
	    do {
                opt = char_to_option((int )*ptr);
                if (opt != 0) {
                    options &= ~opt;
                }
                else {
                    break;
                }
		++ptr;
	    } while (--len > 0);
	}
	if (*ptr == ')') {
	    --len;
	    ++ptr;
	    goto again;
	}
	if (*ptr == ':' && ptr[len-1] == ')') {
	    Regexp *rp;
	    VALUE verbose = ruby_verbose;
	    ruby_verbose = Qfalse;

	    ++ptr;
	    len -= 2;
            err = onig_new(&rp, ptr, ptr + len, ONIG_OPTION_DEFAULT,
			   enc, OnigDefaultSyntax, NULL);
	    onig_free(rp);
	    ruby_verbose = verbose;
	}
	if (err) {
	    options = RREGEXP_PTR(re)->options;
	    ptr = (UChar*)RREGEXP_SRC_PTR(re);
	    len = RREGEXP_SRC_LEN(re);
	}
    }

    if (*option_to_str(optbuf, options)) rb_str_buf_cat2(str, optbuf);

    if ((options & embeddable) != embeddable) {
	optbuf[0] = '-';
	option_to_str(optbuf + 1, ~options);
	rb_str_buf_cat2(str, optbuf);
    }

    rb_str_buf_cat2(str, ":");
    if (rb_enc_asciicompat(enc)) {
	rb_reg_expr_str(str, (char*)ptr, len, enc, NULL);
	rb_str_buf_cat2(str, ")");
    }
    else {
	const char *s, *e;
	char *paren;
	ptrdiff_t n;
	rb_str_buf_cat2(str, ")");
	rb_enc_associate(str, rb_usascii_encoding());
	str = rb_str_encode(str, rb_enc_from_encoding(enc), 0, Qnil);

	/* backup encoded ")" to paren */
	s = RSTRING_PTR(str);
	e = RSTRING_END(str);
	s = rb_enc_left_char_head(s, e-1, e, enc);
	n = e - s;
	paren = ALLOCA_N(char, n);
	memcpy(paren, s, n);
	rb_str_resize(str, RSTRING_LEN(str) - n);

	rb_reg_expr_str(str, (char*)ptr, len, enc, NULL);
	rb_str_buf_cat(str, paren, n);
    }
    rb_enc_copy(str, re);

    OBJ_INFECT(str, re);
    return str;
}