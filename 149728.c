rb_reg_initialize(VALUE obj, const char *s, long len, rb_encoding *enc,
		  int options, onig_errmsg_buffer err,
		  const char *sourcefile, int sourceline)
{
    struct RRegexp *re = RREGEXP(obj);
    VALUE unescaped;
    rb_encoding *fixed_enc = 0;
    rb_encoding *a_enc = rb_ascii8bit_encoding();

    rb_check_frozen(obj);
    if (FL_TEST(obj, REG_LITERAL))
	rb_raise(rb_eSecurityError, "can't modify literal regexp");
    if (re->ptr)
        rb_raise(rb_eTypeError, "already initialized regexp");
    re->ptr = 0;

    if (rb_enc_dummy_p(enc)) {
	errcpy(err, "can't make regexp with dummy encoding");
	return -1;
    }

    unescaped = rb_reg_preprocess(s, s+len, enc, &fixed_enc, err);
    if (unescaped == Qnil)
        return -1;

    if (fixed_enc) {
	if ((fixed_enc != enc && (options & ARG_ENCODING_FIXED)) ||
            (fixed_enc != a_enc && (options & ARG_ENCODING_NONE))) {
	    errcpy(err, "incompatible character encoding");
	    return -1;
	}
        if (fixed_enc != a_enc) {
	    options |= ARG_ENCODING_FIXED;
	    enc = fixed_enc;
	}
    }
    else if (!(options & ARG_ENCODING_FIXED)) {
       enc = rb_usascii_encoding();
    }

    rb_enc_associate((VALUE)re, enc);
    if ((options & ARG_ENCODING_FIXED) || fixed_enc) {
	re->basic.flags |= KCODE_FIXED;
    }
    if (options & ARG_ENCODING_NONE) {
        re->basic.flags |= REG_ENCODING_NONE;
    }

    re->ptr = make_regexp(RSTRING_PTR(unescaped), RSTRING_LEN(unescaped), enc,
			  options & ARG_REG_OPTION_MASK, err,
			  sourcefile, sourceline);
    if (!re->ptr) return -1;
    RB_GC_GUARD(unescaped);
    return 0;
}