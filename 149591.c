rb_memsearch_qs(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *x = xs, *xe = xs + m;
    const unsigned char *y = ys;
    VALUE i, qstable[256];

    /* Preprocessing */
    for (i = 0; i < 256; ++i)
	qstable[i] = m + 1;
    for (; x < xe; ++x)
	qstable[*x] = xe - x;
    /* Searching */
    for (; y + m <= ys + n; y += *(qstable + y[m])) {
	if (*xs == *y && memcmp(xs, y, m) == 0)
	    return y - ys;
    }
    return -1;
}