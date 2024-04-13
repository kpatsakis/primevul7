rb_reg_initialize_m(int argc, VALUE *argv, VALUE self)
{
    int flags = 0;
    VALUE str;
    rb_encoding *enc = 0;

    rb_check_arity(argc, 1, 3);
    if (RB_TYPE_P(argv[0], T_REGEXP)) {
	VALUE re = argv[0];

	if (argc > 1) {
	    rb_warn("flags ignored");
	}
	rb_reg_check(re);
	flags = rb_reg_options(re);
	str = RREGEXP_SRC(re);
    }
    else {
	if (argc >= 2) {
	    if (FIXNUM_P(argv[1])) flags = FIX2INT(argv[1]);
	    else if (RTEST(argv[1])) flags = ONIG_OPTION_IGNORECASE;
	}
	if (argc == 3 && !NIL_P(argv[2])) {
	    char *kcode = StringValuePtr(argv[2]);
	    if (kcode[0] == 'n' || kcode[0] == 'N') {
		enc = rb_ascii8bit_encoding();
		flags |= ARG_ENCODING_NONE;
	    }
	    else {
		rb_warn("encoding option is ignored - %s", kcode);
	    }
	}
	str = StringValue(argv[0]);
    }
    if (enc && rb_enc_get(str) != enc)
	rb_reg_init_str_enc(self, str, enc, flags);
    else
	rb_reg_init_str(self, str, flags);
    return self;
}