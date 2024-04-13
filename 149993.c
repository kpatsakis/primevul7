rb_reg_prepare_enc(VALUE re, VALUE str, int warn)
{
    rb_encoding *enc = 0;
    int cr = str_coderange(str);

    if (cr == ENC_CODERANGE_BROKEN) {
        rb_raise(rb_eArgError,
            "invalid byte sequence in %s",
            rb_enc_name(rb_enc_get(str)));
    }

    rb_reg_check(re);
    enc = rb_enc_get(str);
    if (RREGEXP_PTR(re)->enc == enc) {
    }
    else if (cr == ENC_CODERANGE_7BIT &&
	    RREGEXP_PTR(re)->enc == rb_usascii_encoding()) {
	enc = RREGEXP_PTR(re)->enc;
    }
    else if (!rb_enc_asciicompat(enc)) {
	reg_enc_error(re, str);
    }
    else if (rb_reg_fixed_encoding_p(re)) {
        if ((!rb_enc_asciicompat(RREGEXP_PTR(re)->enc) ||
	     cr != ENC_CODERANGE_7BIT)) {
	    reg_enc_error(re, str);
	}
	enc = RREGEXP_PTR(re)->enc;
    }
    else if (warn && (RBASIC(re)->flags & REG_ENCODING_NONE) &&
	enc != rb_ascii8bit_encoding() &&
	cr != ENC_CODERANGE_7BIT) {
	rb_warn("regexp match /.../n against to %s string",
		rb_enc_name(enc));
    }
    return enc;
}