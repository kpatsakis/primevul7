postprocess_termcap(TERMTYPE2 *tp, bool has_base)
{
    char buf[MAX_LINE * 2 + 2];
    string_desc result;

    /*
     * TERMCAP DEFAULTS AND OBSOLETE-CAPABILITY TRANSLATIONS
     *
     * This first part of the code is the functional inverse of the
     * fragment in capdefaults.c.
     * ----------------------------------------------------------------------
     */

    /* if there was a tc entry, assume we picked up defaults via that */
    if (!has_base) {
	if (WANTED(init_3string) && PRESENT(termcap_init2))
	    init_3string = _nc_save_str(termcap_init2);

	if (WANTED(reset_2string) && PRESENT(termcap_reset))
	    reset_2string = _nc_save_str(termcap_reset);

	if (WANTED(carriage_return)) {
	    if (carriage_return_delay > 0) {
		_nc_SPRINTF(buf, _nc_SLIMIT(sizeof(buf))
			    "%s$<%d>", C_CR, carriage_return_delay);
		carriage_return = _nc_save_str(buf);
	    } else
		carriage_return = _nc_save_str(C_CR);
	}
	if (WANTED(cursor_left)) {
	    if (backspace_delay > 0) {
		_nc_SPRINTF(buf, _nc_SLIMIT(sizeof(buf))
			    "%s$<%d>", C_BS, backspace_delay);
		cursor_left = _nc_save_str(buf);
	    } else if (backspaces_with_bs == 1)
		cursor_left = _nc_save_str(C_BS);
	    else if (PRESENT(backspace_if_not_bs))
		cursor_left = backspace_if_not_bs;
	}
	/* vi doesn't use "do", but it does seem to use nl (or '\n') instead */
	if (WANTED(cursor_down)) {
	    if (PRESENT(linefeed_if_not_lf))
		cursor_down = linefeed_if_not_lf;
	    else if (linefeed_is_newline != 1) {
		if (new_line_delay > 0) {
		    _nc_SPRINTF(buf, _nc_SLIMIT(sizeof(buf))
				"%s$<%d>", C_LF, new_line_delay);
		    cursor_down = _nc_save_str(buf);
		} else
		    cursor_down = _nc_save_str(C_LF);
	    }
	}
	if (WANTED(scroll_forward) && crt_no_scrolling != 1) {
	    if (PRESENT(linefeed_if_not_lf))
		cursor_down = linefeed_if_not_lf;
	    else if (linefeed_is_newline != 1) {
		if (new_line_delay > 0) {
		    _nc_SPRINTF(buf, _nc_SLIMIT(sizeof(buf))
				"%s$<%d>", C_LF, new_line_delay);
		    scroll_forward = _nc_save_str(buf);
		} else
		    scroll_forward = _nc_save_str(C_LF);
	    }
	}
	if (WANTED(newline)) {
	    if (linefeed_is_newline == 1) {
		if (new_line_delay > 0) {
		    _nc_SPRINTF(buf, _nc_SLIMIT(sizeof(buf))
				"%s$<%d>", C_LF, new_line_delay);
		    newline = _nc_save_str(buf);
		} else
		    newline = _nc_save_str(C_LF);
	    } else if (PRESENT(carriage_return) && PRESENT(scroll_forward)) {
		_nc_str_init(&result, buf, sizeof(buf));
		if (_nc_safe_strcat(&result, carriage_return)
		    && _nc_safe_strcat(&result, scroll_forward))
		    newline = _nc_save_str(buf);
	    } else if (PRESENT(carriage_return) && PRESENT(cursor_down)) {
		_nc_str_init(&result, buf, sizeof(buf));
		if (_nc_safe_strcat(&result, carriage_return)
		    && _nc_safe_strcat(&result, cursor_down))
		    newline = _nc_save_str(buf);
	    }
	}
    }

    /*
     * Inverse of capdefaults.c code ends here.
     * ----------------------------------------------------------------------
     *
     * TERMCAP-TO TERMINFO MAPPINGS FOR SOURCE TRANSLATION
     *
     * These translations will *not* be inverted by tgetent().
     */

    if (!has_base) {
	/*
	 * We wait until now to decide if we've got a working cr because even
	 * one that doesn't work can be used for newline. Unfortunately the
	 * space allocated for it is wasted.
	 */
	if (return_does_clr_eol == 1 || no_correctly_working_cr == 1)
	    carriage_return = ABSENT_STRING;

	/*
	 * Supposedly most termcap entries have ta now and '\t' is no longer a
	 * default, but it doesn't seem to be true...
	 */
	if (WANTED(tab)) {
	    if (horizontal_tab_delay > 0) {
		_nc_SPRINTF(buf, _nc_SLIMIT(sizeof(buf))
			    "%s$<%d>", C_HT, horizontal_tab_delay);
		tab = _nc_save_str(buf);
	    } else
		tab = _nc_save_str(C_HT);
	}
	if (init_tabs == ABSENT_NUMERIC && has_hardware_tabs == TRUE)
	    init_tabs = 8;

	/*
	 * Assume we can beep with ^G unless we're given bl@.
	 */
	if (WANTED(bell))
	    bell = _nc_save_str("\007");
    }

    /*
     * Translate the old termcap :pt: capability to it#8 + ht=\t
     */
    if (has_hardware_tabs == TRUE) {
	if (init_tabs != 8 && init_tabs != ABSENT_NUMERIC)
	    _nc_warning("hardware tabs with a width other than 8: %d", init_tabs);
	else {
	    if (PRESENT(tab) && _nc_capcmp(tab, C_HT))
		_nc_warning("hardware tabs with a non-^I tab string %s",
			    _nc_visbuf(tab));
	    else {
		if (WANTED(tab))
		    tab = _nc_save_str(C_HT);
		init_tabs = 8;
	    }
	}
    }
    /*
     * Now translate the ko capability, if there is one.  This
     * isn't from mytinfo...
     */
    if (PRESENT(other_non_function_keys)) {
	char *base;
	char *bp, *cp, *dp;
	struct name_table_entry const *from_ptr;
	struct name_table_entry const *to_ptr;
	char buf2[MAX_TERMINFO_LENGTH];
	bool foundim;

	/* we're going to use this for a special case later */
	dp = strchr(other_non_function_keys, 'i');
	foundim = (dp != 0) && (dp[1] == 'm');

	/* look at each comma-separated capability in the ko string... */
	for (base = other_non_function_keys;
	     (cp = strchr(base, ',')) != 0;
	     base = cp + 1) {
	    size_t len = (unsigned) (cp - base);
	    size_t n;
	    assoc const *ap = 0;

	    for (n = 0; n < SIZEOF(ko_xlate); ++n) {
		if (len == strlen(ko_xlate[n].from)
		    && strncmp(ko_xlate[n].from, base, len) == 0) {
		    ap = ko_xlate + n;
		    break;
		}
	    }
	    if (ap == 0) {
		_nc_warning("unknown capability `%.*s' in ko string",
			    (int) len, base);
		continue;
	    } else if (ap->to[0] == '\0')	/* ignore it */
		continue;

	    /* now we know we found a match in ko_table, so... */

	    from_ptr = _nc_find_entry(ap->from, _nc_get_hash_table(TRUE));
	    to_ptr = _nc_find_entry(ap->to, _nc_get_hash_table(FALSE));

	    if (!from_ptr || !to_ptr)	/* should never happen! */
		_nc_err_abort("ko translation table is invalid, I give up");

	    if (WANTED(tp->Strings[from_ptr->nte_index])) {
		_nc_warning("no value for ko capability %s", ap->from);
		continue;
	    }

	    if (tp->Strings[to_ptr->nte_index]) {
		const char *s = tp->Strings[from_ptr->nte_index];
		const char *t = tp->Strings[to_ptr->nte_index];
		/* There's no point in warning about it if it's the same
		 * string; that's just an inefficiency.
		 */
		if (VALID_STRING(s) && VALID_STRING(t) && strcmp(s, t) != 0)
		    _nc_warning("%s (%s) already has an explicit value %s, ignoring ko",
				ap->to, ap->from, t);
		continue;
	    }

	    /*
	     * The magic moment -- copy the mapped key string over,
	     * stripping out padding.
	     */
	    bp = tp->Strings[from_ptr->nte_index];
	    if (VALID_STRING(bp)) {
		for (dp = buf2; *bp; bp++) {
		    if (bp[0] == '$' && bp[1] == '<') {
			while (*bp && *bp != '>') {
			    ++bp;
			}
		    } else
			*dp++ = *bp;
		}
		*dp = '\0';

		tp->Strings[to_ptr->nte_index] = _nc_save_str(buf2);
	    } else {
		tp->Strings[to_ptr->nte_index] = bp;
	    }
	}

	/*
	 * Note: ko=im and ko=ic both want to grab the `Insert'
	 * keycap.  There's a kich1 but no ksmir, so the ic capability
	 * got mapped to kich1 and im to kIC to avoid a collision.
	 * If the description has im but not ic, hack kIC back to kich1.
	 */
	if (foundim && WANTED(key_ic) && PRESENT(key_sic)) {
	    key_ic = key_sic;
	    key_sic = ABSENT_STRING;
	}
    }

    if (!has_base) {
	if (!hard_copy) {
	    if (WANTED(key_backspace))
		key_backspace = _nc_save_str(C_BS);
	    if (WANTED(key_left))
		key_left = _nc_save_str(C_BS);
	    if (WANTED(key_down))
		key_down = _nc_save_str(C_LF);
	}
    }

    /*
     * Translate XENIX forms characters.
     */
    if (PRESENT(acs_ulcorner) ||
	PRESENT(acs_llcorner) ||
	PRESENT(acs_urcorner) ||
	PRESENT(acs_lrcorner) ||
	PRESENT(acs_ltee) ||
	PRESENT(acs_rtee) ||
	PRESENT(acs_btee) ||
	PRESENT(acs_ttee) ||
	PRESENT(acs_hline) ||
	PRESENT(acs_vline) ||
	PRESENT(acs_plus)) {
	char buf2[MAX_TERMCAP_LENGTH];

	_nc_str_init(&result, buf2, sizeof(buf2));
	_nc_safe_strcat(&result, acs_chars);

	append_acs(&result, 'j', acs_lrcorner);
	append_acs(&result, 'k', acs_urcorner);
	append_acs(&result, 'l', acs_ulcorner);
	append_acs(&result, 'm', acs_llcorner);
	append_acs(&result, 'n', acs_plus);
	append_acs(&result, 'q', acs_hline);
	append_acs(&result, 't', acs_ltee);
	append_acs(&result, 'u', acs_rtee);
	append_acs(&result, 'v', acs_btee);
	append_acs(&result, 'w', acs_ttee);
	append_acs(&result, 'x', acs_vline);

	if (buf2[0]) {
	    acs_chars = _nc_save_str(buf2);
	    _nc_warning("acsc string synthesized from XENIX capabilities");
	}
    } else if (acs_chars == ABSENT_STRING
	       && PRESENT(enter_alt_charset_mode)
	       && PRESENT(exit_alt_charset_mode)) {
	acs_chars = _nc_save_str(VT_ACSC);
    }
}