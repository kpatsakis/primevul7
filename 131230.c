init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    int		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_items, sizeof(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    while (p != NULL)
    {
	*p++ = NUL;
	if (*p == NUL)	    // trailing slash
	    break;
	s = p;
	p = vim_strchr(p, '/');
	if (p == NULL)
	    l = (int)STRLEN(s);
	else
	    l = (int)(p - s);
	if (l >= SY_MAXLEN)
	    return SP_FORMERROR;
	if (ga_grow(&slang->sl_syl_items, 1) == FAIL)
	    return SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_items.ga_data)
					       + slang->sl_syl_items.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    return OK;
}