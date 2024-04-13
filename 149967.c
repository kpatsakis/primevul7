rb_reg_regsub(VALUE str, VALUE src, struct re_registers *regs, VALUE regexp)
{
    VALUE val = 0;
    char *p, *s, *e;
    int no, clen;
    rb_encoding *str_enc = rb_enc_get(str);
    rb_encoding *src_enc = rb_enc_get(src);
    int acompat = rb_enc_asciicompat(str_enc);
#define ASCGET(s,e,cl) (acompat ? (*(cl)=1,ISASCII((s)[0])?(s)[0]:-1) : rb_enc_ascget((s), (e), (cl), str_enc))

    p = s = RSTRING_PTR(str);
    e = s + RSTRING_LEN(str);

    while (s < e) {
        int c = ASCGET(s, e, &clen);
	char *ss;

	if (c == -1) {
	    s += mbclen(s, e, str_enc);
	    continue;
	}
	ss = s;
        s += clen;

	if (c != '\\' || s == e) continue;

	if (!val) {
	    val = rb_str_buf_new(ss-p);
	}
        rb_enc_str_buf_cat(val, p, ss-p, str_enc);

        c = ASCGET(s, e, &clen);
        if (c == -1) {
            s += mbclen(s, e, str_enc);
	    rb_enc_str_buf_cat(val, ss, s-ss, str_enc);
            p = s;
	    continue;
        }
        s += clen;

	p = s;
	switch (c) {
	  case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
            if (!NIL_P(regexp) && onig_noname_group_capture_is_active(RREGEXP_PTR(regexp))) {
                no = c - '0';
            }
            else {
                continue;
            }
	    break;

          case 'k':
            if (s < e && ASCGET(s, e, &clen) == '<') {
                char *name, *name_end;

                name_end = name = s + clen;
                while (name_end < e) {
                    c = ASCGET(name_end, e, &clen);
                    if (c == '>') break;
                    name_end += c == -1 ? mbclen(name_end, e, str_enc) : clen;
                }
                if (name_end < e) {
		    VALUE n = rb_str_subseq(str, (long)(name - RSTRING_PTR(str)),
					    (long)(name_end - name));
		    if (!rb_enc_compatible(RREGEXP_SRC(regexp), n) ||
			(no = name_to_backref_number(regs, regexp, name, name_end)) < 1) {
			name_to_backref_error(n);
		    }
                    p = s = name_end + clen;
                    break;
                }
                else {
                    rb_raise(rb_eRuntimeError, "invalid group name reference format");
                }
            }

            rb_enc_str_buf_cat(val, ss, s-ss, str_enc);
            continue;

          case '0':
	  case '&':
	    no = 0;
	    break;

	  case '`':
	    rb_enc_str_buf_cat(val, RSTRING_PTR(src), BEG(0), src_enc);
	    continue;

	  case '\'':
	    rb_enc_str_buf_cat(val, RSTRING_PTR(src)+END(0), RSTRING_LEN(src)-END(0), src_enc);
	    continue;

	  case '+':
	    no = regs->num_regs-1;
	    while (BEG(no) == -1 && no > 0) no--;
	    if (no == 0) continue;
	    break;

	  case '\\':
	    rb_enc_str_buf_cat(val, s-clen, clen, str_enc);
	    continue;

	  default:
	    rb_enc_str_buf_cat(val, ss, s-ss, str_enc);
	    continue;
	}

	if (no >= 0) {
	    if (no >= regs->num_regs) continue;
	    if (BEG(no) == -1) continue;
	    rb_enc_str_buf_cat(val, RSTRING_PTR(src)+BEG(no), END(no)-BEG(no), src_enc);
	}
    }

    if (!val) return str;
    if (p < e) {
        rb_enc_str_buf_cat(val, p, e-p, str_enc);
    }

    return val;
}