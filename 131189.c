spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    int		word[MAXWLEN];
    int		wres[MAXWLEN];
    int		l;
    char_u	*s;
    int		*ws;
    char_u	*t;
    int		*pf;
    int		i, j, z;
    int		reslen;
    int		n, k = 0;
    int		z0;
    int		k0;
    int		n0;
    int		c;
    int		pri;
    int		p0 = -333;
    int		c0;
    int		did_white = FALSE;
    int		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    for (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	if (slang->sl_rem_accents)
	{
	    if (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		if (did_white)
		    continue;
		c = ' ';
		did_white = TRUE;
	    }
	    else
	    {
		did_white = FALSE;
		if (!spell_iswordp_nmw(t, curwin))
		    continue;
	    }
	}
	word[wordlen++] = c;
    }
    word[wordlen] = NUL;

    /*
     * This algorithm comes from Aspell phonet.cpp.
     * Converted from C++ to C.  Added support for multi-byte chars.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    while ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	if (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    for (; ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		if (c != ws[0])
		    continue;
		k = smp[n].sm_leadlen;
		if (k > 1)
		{
		    if (word[i + 1] != ws[1])
			continue;
		    if (k > 2)
		    {
			for (j = 2; j < k; ++j)
			    if (word[i + j] != ws[j])
				break;
			if (j < k)
			    continue;
		    }
		}

		if ((pf = smp[n].sm_oneof_w) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    while (*pf != NUL && *pf != word[i + k])
			++pf;
		    if (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		while (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		if (*s == '<')
		    s++;
		if (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		if (*s == '^' && *(s + 1) == '^')
		    s++;

		if (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(word[i - 1] == ' '
				    || spell_iswordp_w(word + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp_w(word + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp_w(word + i - 1, curwin)
			    && (!spell_iswordp_w(word + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = word[i + k - 1];
		    n0 = slang->sl_sal_first[c0 & 0xff];

		    if (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			for ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    if (c0 != ws[0])
				continue;
			    k0 = smp[n0].sm_leadlen;
			    if (k0 > 1)
			    {
				if (word[i + k] != ws[1])
				    continue;
				if (k0 > 2)
				{
				    pf = word + i + k + 1;
				    for (j = 2; j < k0; ++j)
					if (*pf++ != ws[j])
					    break;
				    if (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    if ((pf = smp[n0].sm_oneof_w) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				while (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				if (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    while (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    if (*s == '<')
				s++;
			    if (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    if (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				if (k0 == k)
				    // this is just a piece of the string
				    continue;

				if (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				break;
			    }
			}

			if (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    continue;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != NULL) ? 1 : 0;
		    if (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			if (reslen > 0 && ws != NULL && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			if (ws != NULL)
			    while (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			if (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    sizeof(int) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    else
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			if (ws != NULL)
			    while (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				if (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			if (ws == NULL)
			    c = NUL;
			else
			    c = *ws;
			if (strstr((char *)s, "^^") != NULL)
			{
			    if (c != NUL)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       sizeof(int) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    break;
		}
	    }
	}
	else if (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	if (z0 == 0)
	{
	    if (k && !p0 && reslen < MAXWLEN && c != NUL
		    && (!slang->sl_collapse || reslen == 0
						     || wres[reslen - 1] != c))
		// condense only double letters
		wres[reslen++] = c;

	    i++;
	    z = 0;
	    k = 0;
	}
    }

    // Convert wide characters in "wres" to a multi-byte string in "res".
    l = 0;
    for (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	if (l + MB_MAXBYTES > MAXWLEN)
	    break;
    }
    res[l] = NUL;
}