rb_memsearch(const void *x0, long m, const void *y0, long n, rb_encoding *enc)
{
    const unsigned char *x = x0, *y = y0;

    if (m > n) return -1;
    else if (m == n) {
	return memcmp(x0, y0, m) == 0 ? 0 : -1;
    }
    else if (m < 1) {
	return 0;
    }
    else if (m == 1) {
	const unsigned char *ys = memchr(y, *x, n);

	if (ys)
	    return ys - y;
	else
	    return -1;
    }
    else if (LIKELY(rb_enc_mbminlen(enc) == 1)) {
	if (m <= SIZEOF_VALUE) {
	    return rb_memsearch_ss(x0, m, y0, n);
	}
	else if (enc == rb_utf8_encoding()){
	    return rb_memsearch_qs_utf8(x0, m, y0, n);
	}
    }
    else if (LIKELY(rb_enc_mbminlen(enc) == 2)) {
	return rb_memsearch_wchar(x0, m, y0, n);
    }
    else if (LIKELY(rb_enc_mbminlen(enc) == 4)) {
	return rb_memsearch_qchar(x0, m, y0, n);
    }
    return rb_memsearch_qs(x0, m, y0, n);
}