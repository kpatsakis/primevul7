rb_reg_preprocess_dregexp(VALUE ary, int options)
{
    rb_encoding *fixed_enc = 0;
    rb_encoding *regexp_enc = 0;
    onig_errmsg_buffer err = "";
    int i;
    VALUE result = 0;
    rb_encoding *ascii8bit = rb_ascii8bit_encoding();

    if (RARRAY_LEN(ary) == 0) {
        rb_raise(rb_eArgError, "no arguments given");
    }

    for (i = 0; i < RARRAY_LEN(ary); i++) {
        VALUE str = RARRAY_AREF(ary, i);
        VALUE buf;
        char *p, *end;
        rb_encoding *src_enc;

	src_enc = rb_enc_get(str);
	if (options & ARG_ENCODING_NONE &&
		src_enc != ascii8bit) {
	    if (str_coderange(str) != ENC_CODERANGE_7BIT)
		rb_raise(rb_eRegexpError, "/.../n has a non escaped non ASCII character in non ASCII-8BIT script");
	    else
		src_enc = ascii8bit;
	}

        StringValue(str);
        p = RSTRING_PTR(str);
        end = p + RSTRING_LEN(str);

        buf = rb_reg_preprocess(p, end, src_enc, &fixed_enc, err);

        if (buf == Qnil)
            rb_raise(rb_eArgError, "%s", err);

        if (fixed_enc != 0) {
            if (regexp_enc != 0 && regexp_enc != fixed_enc) {
                rb_raise(rb_eRegexpError, "encoding mismatch in dynamic regexp : %s and %s",
                         rb_enc_name(regexp_enc), rb_enc_name(fixed_enc));
            }
            regexp_enc = fixed_enc;
        }

        if (!result)
            result = rb_str_new3(str);
        else
            rb_str_buf_append(result, str);
    }
    if (regexp_enc) {
        rb_enc_associate(result, regexp_enc);
    }

    return result;
}