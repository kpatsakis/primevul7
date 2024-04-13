unescape_escaped_nonascii(const char **pp, const char *end, rb_encoding *enc,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    const char *p = *pp;
    int chmaxlen = rb_enc_mbmaxlen(enc);
    char *chbuf = ALLOCA_N(char, chmaxlen);
    int chlen = 0;
    int byte;
    int l;

    memset(chbuf, 0, chmaxlen);

    byte = read_escaped_byte(&p, end, err);
    if (byte == -1) {
        return -1;
    }

    chbuf[chlen++] = byte;
    while (chlen < chmaxlen &&
           MBCLEN_NEEDMORE_P(rb_enc_precise_mbclen(chbuf, chbuf+chlen, enc))) {
        byte = read_escaped_byte(&p, end, err);
        if (byte == -1) {
            return -1;
        }
        chbuf[chlen++] = byte;
    }

    l = rb_enc_precise_mbclen(chbuf, chbuf+chlen, enc);
    if (MBCLEN_INVALID_P(l)) {
        errcpy(err, "invalid multibyte escape");
        return -1;
    }
    if (1 < chlen || (chbuf[0] & 0x80)) {
        rb_str_buf_cat(buf, chbuf, chlen);

        if (*encp == 0)
            *encp = enc;
        else if (*encp != enc) {
            errcpy(err, "escaped non ASCII character in UTF-8 regexp");
            return -1;
        }
    }
    else {
        char escbuf[5];
        snprintf(escbuf, sizeof(escbuf), "\\x%02X", chbuf[0]&0xff);
        rb_str_buf_cat(buf, escbuf, 4);
    }
    *pp = p;
    return 0;
}