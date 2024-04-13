rb_reg_s_union(VALUE self, VALUE args0)
{
    long argc = RARRAY_LEN(args0);

    if (argc == 0) {
        VALUE args[1];
        args[0] = rb_str_new2("(?!)");
        return rb_class_new_instance(1, args, rb_cRegexp);
    }
    else if (argc == 1) {
        VALUE arg = rb_ary_entry(args0, 0);
        VALUE re = rb_check_regexp_type(arg);
        if (!NIL_P(re))
            return re;
        else {
            VALUE quoted;
            quoted = rb_reg_s_quote(Qnil, arg);
            return rb_reg_new_str(quoted, 0);
        }
    }
    else {
	int i;
	VALUE source = rb_str_buf_new(0);
	rb_encoding *result_enc;

        int has_asciionly = 0;
        rb_encoding *has_ascii_compat_fixed = 0;
        rb_encoding *has_ascii_incompat = 0;

	for (i = 0; i < argc; i++) {
	    volatile VALUE v;
	    VALUE e = rb_ary_entry(args0, i);

	    if (0 < i)
		rb_str_buf_cat_ascii(source, "|");

	    v = rb_check_regexp_type(e);
	    if (!NIL_P(v)) {
                rb_encoding *enc = rb_enc_get(v);
                if (!rb_enc_asciicompat(enc)) {
                    if (!has_ascii_incompat)
                        has_ascii_incompat = enc;
                    else if (has_ascii_incompat != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_incompat), rb_enc_name(enc));
                }
                else if (rb_reg_fixed_encoding_p(v)) {
                    if (!has_ascii_compat_fixed)
                        has_ascii_compat_fixed = enc;
                    else if (has_ascii_compat_fixed != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_compat_fixed), rb_enc_name(enc));
                }
                else {
                    has_asciionly = 1;
                }
		v = rb_reg_to_s(v);
	    }
	    else {
                rb_encoding *enc;
                StringValue(e);
                enc = rb_enc_get(e);
                if (!rb_enc_asciicompat(enc)) {
                    if (!has_ascii_incompat)
                        has_ascii_incompat = enc;
                    else if (has_ascii_incompat != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_incompat), rb_enc_name(enc));
                }
                else if (rb_enc_str_asciionly_p(e)) {
                    has_asciionly = 1;
                }
                else {
                    if (!has_ascii_compat_fixed)
                        has_ascii_compat_fixed = enc;
                    else if (has_ascii_compat_fixed != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_compat_fixed), rb_enc_name(enc));
                }
		v = rb_reg_s_quote(Qnil, e);
	    }
            if (has_ascii_incompat) {
                if (has_asciionly) {
                    rb_raise(rb_eArgError, "ASCII incompatible encoding: %s",
                        rb_enc_name(has_ascii_incompat));
                }
                if (has_ascii_compat_fixed) {
                    rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                        rb_enc_name(has_ascii_incompat), rb_enc_name(has_ascii_compat_fixed));
                }
            }

            if (i == 0) {
                rb_enc_copy(source, v);
            }
	    rb_str_append(source, v);
	}

        if (has_ascii_incompat) {
            result_enc = has_ascii_incompat;
        }
        else if (has_ascii_compat_fixed) {
            result_enc = has_ascii_compat_fixed;
        }
        else {
            result_enc = rb_ascii8bit_encoding();
        }

        rb_enc_associate(source, result_enc);
        return rb_class_new_instance(1, &source, rb_cRegexp);
    }
}