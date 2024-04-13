unescape_nonascii(const char *p, const char *end, rb_encoding *enc,
        VALUE buf, rb_encoding **encp, int *has_property,
        onig_errmsg_buffer err)
{
    char c;
    char smallbuf[2];

    while (p < end) {
        int chlen = rb_enc_precise_mbclen(p, end, enc);
        if (!MBCLEN_CHARFOUND_P(chlen)) {
            errcpy(err, "invalid multibyte character");
            return -1;
        }
        chlen = MBCLEN_CHARFOUND_LEN(chlen);
        if (1 < chlen || (*p & 0x80)) {
            rb_str_buf_cat(buf, p, chlen);
            p += chlen;
            if (*encp == 0)
                *encp = enc;
            else if (*encp != enc) {
                errcpy(err, "non ASCII character in UTF-8 regexp");
                return -1;
            }
            continue;
        }

        switch (c = *p++) {
          case '\\':
            if (p == end) {
                errcpy(err, "too short escape sequence");
                return -1;
            }
            switch (c = *p++) {
              case '1': case '2': case '3':
              case '4': case '5': case '6': case '7': /* \O, \OO, \OOO or backref */
                {
                    size_t len = end-(p-1), octlen;
                    if (ruby_scan_oct(p-1, len < 3 ? len : 3, &octlen) <= 0177) {
                        /* backref or 7bit octal.
                           no need to unescape anyway.
                           re-escaping may break backref */
                        goto escape_asis;
                    }
                }
                /* xxx: How about more than 199 subexpressions? */

              case '0': /* \0, \0O, \0OO */

              case 'x': /* \xHH */
              case 'c': /* \cX, \c\M-X */
              case 'C': /* \C-X, \C-\M-X */
              case 'M': /* \M-X, \M-\C-X, \M-\cX */
                p = p-2;
		if (enc == rb_usascii_encoding()) {
		    const char *pbeg = p;
		    c = read_escaped_byte(&p, end, err);
		    if (c == (char)-1) return -1;
		    rb_str_buf_cat(buf, pbeg, p-pbeg);
		}
		else {
		    if (unescape_escaped_nonascii(&p, end, enc, buf, encp, err) != 0)
			return -1;
		}
                break;

              case 'u':
                if (p == end) {
                    errcpy(err, "too short escape sequence");
                    return -1;
                }
                if (*p == '{') {
                    /* \u{H HH HHH HHHH HHHHH HHHHHH ...} */
                    p++;
                    if (unescape_unicode_list(&p, end, buf, encp, err) != 0)
                        return -1;
                    if (p == end || *p++ != '}') {
                        errcpy(err, "invalid Unicode list");
                        return -1;
                    }
                    break;
                }
                else {
                    /* \uHHHH */
                    if (unescape_unicode_bmp(&p, end, buf, encp, err) != 0)
                        return -1;
                    break;
                }

              case 'p': /* \p{Hiragana} */
              case 'P':
                if (!*encp) {
                    *has_property = 1;
                }
                goto escape_asis;

              default: /* \n, \\, \d, \9, etc. */
escape_asis:
                smallbuf[0] = '\\';
                smallbuf[1] = c;
                rb_str_buf_cat(buf, smallbuf, 2);
                break;
            }
            break;

          default:
            rb_str_buf_cat(buf, &c, 1);
            break;
        }
    }

    return 0;
}