rb_reg_expr_str(VALUE str, const char *s, long len,
	rb_encoding *enc, rb_encoding *resenc)
{
    const char *p, *pend;
    int cr = ENC_CODERANGE_UNKNOWN;
    int need_escape = 0;
    int c, clen;

    p = s; pend = p + len;
    rb_str_coderange_scan_restartable(p, pend, enc, &cr);
    if (rb_enc_asciicompat(enc) && ENC_CODERANGE_CLEAN_P(cr)) {
	while (p < pend) {
	    c = rb_enc_ascget(p, pend, &clen, enc);
	    if (c == -1) {
		if (enc == resenc) {
		    p += mbclen(p, pend, enc);
		}
		else {
		    need_escape = 1;
		    break;
		}
	    }
	    else if (c != '/' && rb_enc_isprint(c, enc)) {
		p += clen;
	    }
	    else {
		need_escape = 1;
		break;
	    }
	}
    }
    else {
	need_escape = 1;
    }

    if (!need_escape) {
	rb_str_buf_cat(str, s, len);
    }
    else {
	int unicode_p = rb_enc_unicode_p(enc);
	p = s;
	while (p<pend) {
            c = rb_enc_ascget(p, pend, &clen, enc);
	    if (c == '\\' && p+clen < pend) {
		int n = clen + mbclen(p+clen, pend, enc);
		rb_str_buf_cat(str, p, n);
		p += n;
		continue;
	    }
	    else if (c == '/') {
		char c = '\\';
		rb_str_buf_cat(str, &c, 1);
		rb_str_buf_cat(str, p, clen);
	    }
	    else if (c == -1) {
		clen = rb_enc_precise_mbclen(p, pend, enc);
		if (!MBCLEN_CHARFOUND_P(clen)) {
		    c = (unsigned char)*p;
		    clen = 1;
		    goto hex;
		}
		if (resenc) {
		    unsigned int c = rb_enc_mbc_to_codepoint(p, pend, enc);
		    rb_str_buf_cat_escaped_char(str, c, unicode_p);
		}
		else {
		    clen = MBCLEN_CHARFOUND_LEN(clen);
		    rb_str_buf_cat(str, p, clen);
		}
	    }
	    else if (rb_enc_isprint(c, enc)) {
		rb_str_buf_cat(str, p, clen);
	    }
	    else if (!rb_enc_isspace(c, enc)) {
		char b[8];

	      hex:
		snprintf(b, sizeof(b), "\\x%02X", c);
		rb_str_buf_cat(str, b, 4);
	    }
	    else {
		rb_str_buf_cat(str, p, clen);
	    }
	    p += clen;
	}
    }
}