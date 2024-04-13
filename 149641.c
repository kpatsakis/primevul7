unescape_unicode_list(const char **pp, const char *end,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    const char *p = *pp;
    int has_unicode = 0;
    unsigned long code;
    size_t len;

    while (p < end && ISSPACE(*p)) p++;

    while (1) {
        code = ruby_scan_hex(p, end-p, &len);
        if (len == 0)
            break;
        if (6 < len) { /* max 10FFFF */
            errcpy(err, "invalid Unicode range");
            return -1;
        }
        p += len;
        if (append_utf8(code, buf, encp, err) != 0)
            return -1;
        has_unicode = 1;

        while (p < end && ISSPACE(*p)) p++;
    }

    if (has_unicode == 0) {
        errcpy(err, "invalid Unicode list");
        return -1;
    }

    *pp = p;

    return 0;
}