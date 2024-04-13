rb_reg_quote(VALUE str)
{
    rb_encoding *enc = rb_enc_get(str);
    char *s, *send, *t;
    VALUE tmp;
    int c, clen;
    int ascii_only = rb_enc_str_asciionly_p(str);

    s = RSTRING_PTR(str);
    send = s + RSTRING_LEN(str);
    while (s < send) {
        c = rb_enc_ascget(s, send, &clen, enc);
	if (c == -1) {
            s += mbclen(s, send, enc);
	    continue;
	}
	switch (c) {
	  case '[': case ']': case '{': case '}':
	  case '(': case ')': case '|': case '-':
	  case '*': case '.': case '\\':
	  case '?': case '+': case '^': case '$':
	  case ' ': case '#':
	  case '\t': case '\f': case '\v': case '\n': case '\r':
	    goto meta_found;
	}
        s += clen;
    }
    tmp = rb_str_new3(str);
    if (ascii_only) {
        rb_enc_associate(tmp, rb_usascii_encoding());
    }
    return tmp;

  meta_found:
    tmp = rb_str_new(0, RSTRING_LEN(str)*2);
    if (ascii_only) {
        rb_enc_associate(tmp, rb_usascii_encoding());
    }
    else {
        rb_enc_copy(tmp, str);
    }
    t = RSTRING_PTR(tmp);
    /* copy upto metacharacter */
    memcpy(t, RSTRING_PTR(str), s - RSTRING_PTR(str));
    t += s - RSTRING_PTR(str);

    while (s < send) {
        c = rb_enc_ascget(s, send, &clen, enc);
	if (c == -1) {
	    int n = mbclen(s, send, enc);

	    while (n--)
		*t++ = *s++;
	    continue;
	}
        s += clen;
	switch (c) {
	  case '[': case ']': case '{': case '}':
	  case '(': case ')': case '|': case '-':
	  case '*': case '.': case '\\':
	  case '?': case '+': case '^': case '$':
	  case '#':
            t += rb_enc_mbcput('\\', t, enc);
	    break;
	  case ' ':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput(' ', t, enc);
	    continue;
	  case '\t':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('t', t, enc);
	    continue;
	  case '\n':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('n', t, enc);
	    continue;
	  case '\r':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('r', t, enc);
	    continue;
	  case '\f':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('f', t, enc);
	    continue;
	  case '\v':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('v', t, enc);
	    continue;
	}
        t += rb_enc_mbcput(c, t, enc);
    }
    rb_str_resize(tmp, t - RSTRING_PTR(tmp));
    OBJ_INFECT(tmp, str);
    return tmp;
}