rb_reg_adjust_startpos(VALUE re, VALUE str, long pos, int reverse)
{
    long range;
    rb_encoding *enc;
    UChar *p, *string;

    enc = rb_reg_prepare_enc(re, str, 0);

    if (reverse) {
	range = -pos;
    }
    else {
	range = RSTRING_LEN(str) - pos;
    }

    if (pos > 0 && ONIGENC_MBC_MAXLEN(enc) != 1 && pos < RSTRING_LEN(str)) {
	 string = (UChar*)RSTRING_PTR(str);

	 if (range > 0) {
	      p = onigenc_get_right_adjust_char_head(enc, string, string + pos, string + RSTRING_LEN(str));
	 }
	 else {
	      p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, string, string + pos, string + RSTRING_LEN(str));
	 }
	 return p - string;
    }

    return pos;
}