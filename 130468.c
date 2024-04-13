dump_entry(TERMTYPE2 *tterm,
	   int suppress_untranslatable,
	   int limited,
	   int numbers,
	   PredFunc pred)
{
    TERMTYPE2 save_tterm;
    int len, critlen;
    const char *legend;
    bool infodump;

    if (quickdump) {
	char bigbuf[65536];
	unsigned n;
	unsigned offset = 0;
	separator = "";
	trailer = "\n";
	indent = 0;
	if (_nc_write_object(tterm, bigbuf, &offset, sizeof(bigbuf)) == OK) {
	    char numbuf[80];
	    if (quickdump & 1) {
		if (outbuf.used)
		    wrap_concat1("\n");
		wrap_concat1("hex:");
		for (n = 0; n < offset; ++n) {
		    _nc_SPRINTF(numbuf, _nc_SLIMIT(sizeof(numbuf))
				"%02X", UChar(bigbuf[n]));
		    wrap_concat1(numbuf);
		}
	    }
	    if (quickdump & 2) {
		static char padding[] =
		{0, 0};
		int value = 0;
		if (outbuf.used)
		    wrap_concat1("\n");
		wrap_concat1("b64:");
		for (n = 0; n < offset; ++n) {
		    encode_b64(numbuf, bigbuf, n, &value);
		    wrap_concat1(numbuf);
		}
		switch (n % 3) {
		case 0:
		    break;
		case 1:
		    encode_b64(numbuf, padding, 1, &value);
		    wrap_concat1(numbuf);
		    wrap_concat1("==");
		    break;
		case 2:
		    encode_b64(numbuf, padding, 1, &value);
		    wrap_concat1(numbuf);
		    wrap_concat1("=");
		    break;
		}
	    }
	}
	return;
    }

    if (TcOutput()) {
	critlen = MAX_TERMCAP_LENGTH;
	legend = "older termcap";
	infodump = FALSE;
	set_obsolete_termcaps(tterm);
    } else {
	critlen = MAX_TERMINFO_LENGTH;
	legend = "terminfo";
	infodump = TRUE;
    }

    save_sgr = set_attributes;

    if ((FMT_ENTRY() > critlen)
	&& limited) {

	save_tterm = *tterm;
	if (!suppress_untranslatable) {
	    SHOW_WHY("# (untranslatable capabilities removed to fit entry within %d bytes)\n",
		     critlen);
	    suppress_untranslatable = TRUE;
	}
	if (FMT_ENTRY() > critlen) {
	    /*
	     * We pick on sgr because it's a nice long string capability that
	     * is really just an optimization hack.  Another good candidate is
	     * acsc since it is both long and unused by BSD termcap.
	     */
	    bool changed = FALSE;

#if NCURSES_XNAMES
	    /*
	     * Extended names are most likely function-key definitions.  Drop
	     * those first.
	     */
	    unsigned n;
	    for (n = STRCOUNT; n < NUM_STRINGS(tterm); n++) {
		const char *name = ExtStrname(tterm, (int) n, strnames);

		if (VALID_STRING(tterm->Strings[n])) {
		    set_attributes = ABSENT_STRING;
		    /* we remove long names anyway - only report the short */
		    if (strlen(name) <= 2) {
			SHOW_WHY("# (%s removed to fit entry within %d bytes)\n",
				 name,
				 critlen);
		    }
		    changed = TRUE;
		    if (FMT_ENTRY() <= critlen)
			break;
		}
	    }
#endif
	    if (VALID_STRING(set_attributes)) {
		set_attributes = ABSENT_STRING;
		SHOW_WHY("# (sgr removed to fit entry within %d bytes)\n",
			 critlen);
		changed = TRUE;
	    }
	    if (!changed || (FMT_ENTRY() > critlen)) {
		if (purged_acs(tterm)) {
		    acs_chars = ABSENT_STRING;
		    SHOW_WHY("# (acsc removed to fit entry within %d bytes)\n",
			     critlen);
		    changed = TRUE;
		}
	    }
	    if (!changed || (FMT_ENTRY() > critlen)) {
		int oldversion = tversion;

		tversion = V_BSD;
		SHOW_WHY("# (terminfo-only capabilities suppressed to fit entry within %d bytes)\n",
			 critlen);

		len = FMT_ENTRY();
		if (len > critlen
		    && kill_labels(tterm, len - critlen)) {
		    SHOW_WHY("# (some labels capabilities suppressed to fit entry within %d bytes)\n",
			     critlen);
		    len = FMT_ENTRY();
		}
		if (len > critlen
		    && kill_fkeys(tterm, len - critlen)) {
		    SHOW_WHY("# (some function-key capabilities suppressed to fit entry within %d bytes)\n",
			     critlen);
		    len = FMT_ENTRY();
		}
		if (len > critlen) {
		    (void) fprintf(stderr,
				   "%s: %s entry is %d bytes long\n",
				   _nc_progname,
				   _nc_first_name(tterm->term_names),
				   len);
		    SHOW_WHY("# WARNING: this entry, %d bytes long, may core-dump %s libraries!\n",
			     len, legend);
		}
		tversion = oldversion;
	    }
	    set_attributes = save_sgr;
	    *tterm = save_tterm;
	}
    } else if (!version_filter(STRING, STR_IDX(acs_chars))) {
	save_tterm = *tterm;
	if (purged_acs(tterm)) {
	    (void) FMT_ENTRY();
	}
	*tterm = save_tterm;
    }
}