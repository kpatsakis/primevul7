count_common_word(
    slang_T	*lp,
    char_u	*word,
    int		len,	    // word length, -1 for up to NUL
    int		count)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    if (len == -1)
	p = word;
    else if (len >= MAXWLEN)
	return;
    else
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    if (HASHITEM_EMPTY(hi))
    {
	wc = alloc(sizeof(wordcount_T) + STRLEN(p));
	if (wc == NULL)
	    return;
	STRCPY(wc->wc_word, p);
	wc->wc_count = count;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    else
    {
	wc = HI2WC(hi);
	if ((wc->wc_count += count) < (unsigned)count)	// check for overflow
	    wc->wc_count = MAXWORDCOUNT;
    }
}