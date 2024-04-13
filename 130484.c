_nc_parse_entry(ENTRY * entryp, int literal, bool silent)
{
    int token_type;
    struct name_table_entry const *entry_ptr;
    char *ptr, *base;
    const char *name;
    bool bad_tc_usage = FALSE;

    token_type = _nc_get_token(silent);

    if (token_type == EOF)
	return (EOF);
    if (token_type != NAMES)
	_nc_err_abort("Entry does not start with terminal names in column one");

    _nc_init_entry(entryp);

    entryp->cstart = _nc_comment_start;
    entryp->cend = _nc_comment_end;
    entryp->startline = _nc_start_line;
    DEBUG(2, ("Comment range is %ld to %ld", entryp->cstart, entryp->cend));

    /*
     * Strip off the 2-character termcap name, if present.  Originally termcap
     * used that as an indexing aid.  We can retain 2-character terminfo names,
     * but note that they would be lost if we translate to/from termcap.  This
     * feature is supposedly obsolete since "newer" BSD implementations do not
     * use it; however our reference for this feature is SunOS 4.x, which
     * implemented it.  Note that the resulting terminal type was never the
     * 2-character name, but was instead the first alias after that.
     */
#define ok_TC2(s) (isgraph(UChar(s)) && (s) != '|')
    ptr = _nc_curr_token.tk_name;
    if (_nc_syntax == SYN_TERMCAP
#if NCURSES_XNAMES
	&& !_nc_user_definable
#endif
	) {
	if (ok_TC2(ptr[0]) && ok_TC2(ptr[1]) && (ptr[2] == '|')) {
	    ptr += 3;
	    _nc_curr_token.tk_name[2] = '\0';
	}
    }

    entryp->tterm.str_table = entryp->tterm.term_names = _nc_save_str(ptr);

    if (entryp->tterm.str_table == 0)
	return (ERR);

    DEBUG(1, ("Starting '%s'", ptr));

    /*
     * We do this because the one-token lookahead in the parse loop
     * results in the terminal type getting prematurely set to correspond
     * to that of the next entry.
     */
    name = _nc_first_name(entryp->tterm.term_names);
    if (!valid_entryname(name)) {
	_nc_warning("invalid entry name \"%s\"", name);
	name = "invalid";
    }
    _nc_set_type(name);

    /* check for overly-long names and aliases */
    for (base = entryp->tterm.term_names; (ptr = strchr(base, '|')) != 0;
	 base = ptr + 1) {
	if (ptr - base > MAX_ALIAS) {
	    _nc_warning("%s `%.*s' may be too long",
			(base == entryp->tterm.term_names)
			? "primary name"
			: "alias",
			(int) (ptr - base), base);
	}
    }

    entryp->nuses = 0;

    for (token_type = _nc_get_token(silent);
	 token_type != EOF && token_type != NAMES;
	 token_type = _nc_get_token(silent)) {
	bool is_use = (strcmp(_nc_curr_token.tk_name, "use") == 0);
	bool is_tc = !is_use && (strcmp(_nc_curr_token.tk_name, "tc") == 0);
	if (is_use || is_tc) {
	    if (!VALID_STRING(_nc_curr_token.tk_valstring)
		|| _nc_curr_token.tk_valstring[0] == '\0') {
		_nc_warning("missing name for use-clause");
		continue;
	    } else if (!valid_entryname(_nc_curr_token.tk_valstring)) {
		_nc_warning("invalid name for use-clause \"%s\"",
			    _nc_curr_token.tk_valstring);
		continue;
	    } else if (entryp->nuses >= MAX_USES) {
		_nc_warning("too many use-clauses, ignored \"%s\"",
			    _nc_curr_token.tk_valstring);
		continue;
	    }
	    entryp->uses[entryp->nuses].name = _nc_save_str(_nc_curr_token.tk_valstring);
	    entryp->uses[entryp->nuses].line = _nc_curr_line;
	    entryp->nuses++;
	    if (entryp->nuses > 1 && is_tc) {
		BAD_TC_USAGE
	    }
	} else {
	    /* normal token lookup */
	    entry_ptr = _nc_find_entry(_nc_curr_token.tk_name,
				       _nc_get_hash_table(_nc_syntax));

	    /*
	     * Our kluge to handle aliasing.  The reason it's done
	     * this ugly way, with a linear search, is so the hashing
	     * machinery doesn't have to be made really complicated
	     * (also we get better warnings this way).  No point in
	     * making this case fast, aliased caps aren't common now
	     * and will get rarer.
	     */
	    if (entry_ptr == NOTFOUND) {
		const struct alias *ap;

		if (_nc_syntax == SYN_TERMCAP) {
		    if (entryp->nuses != 0) {
			BAD_TC_USAGE
		    }
		    for (ap = _nc_get_alias_table(TRUE); ap->from; ap++)
			if (strcmp(ap->from, _nc_curr_token.tk_name) == 0) {
			    if (ap->to == (char *) 0) {
				_nc_warning("%s (%s termcap extension) ignored",
					    ap->from, ap->source);
				goto nexttok;
			    }

			    entry_ptr = _nc_find_entry(ap->to,
						       _nc_get_hash_table(TRUE));
			    if (entry_ptr && !silent)
				_nc_warning("%s (%s termcap extension) aliased to %s",
					    ap->from, ap->source, ap->to);
			    break;
			}
		} else {	/* if (_nc_syntax == SYN_TERMINFO) */
		    for (ap = _nc_get_alias_table(FALSE); ap->from; ap++)
			if (strcmp(ap->from, _nc_curr_token.tk_name) == 0) {
			    if (ap->to == (char *) 0) {
				_nc_warning("%s (%s terminfo extension) ignored",
					    ap->from, ap->source);
				goto nexttok;
			    }

			    entry_ptr = _nc_find_entry(ap->to,
						       _nc_get_hash_table(FALSE));
			    if (entry_ptr && !silent)
				_nc_warning("%s (%s terminfo extension) aliased to %s",
					    ap->from, ap->source, ap->to);
			    break;
			}

		    if (entry_ptr == NOTFOUND) {
			entry_ptr = lookup_fullname(_nc_curr_token.tk_name);
		    }
		}
	    }
#if NCURSES_XNAMES
	    /*
	     * If we have extended-names active, we will automatically
	     * define a name based on its context.
	     */
	    if (entry_ptr == NOTFOUND
		&& _nc_user_definable) {
		if (expected_type(_nc_curr_token.tk_name, token_type, silent)) {
		    if ((entry_ptr = _nc_extend_names(entryp,
						      _nc_curr_token.tk_name,
						      token_type)) != 0) {
			if (_nc_tracing >= DEBUG_LEVEL(1)) {
			    _nc_warning("extended capability '%s'",
					_nc_curr_token.tk_name);
			}
		    }
		} else {
		    /* ignore it: we have already printed error message */
		    continue;
		}
	    }
#endif /* NCURSES_XNAMES */

	    /* can't find this cap name, not even as an alias */
	    if (entry_ptr == NOTFOUND) {
		if (!silent)
		    _nc_warning("unknown capability '%s'",
				_nc_curr_token.tk_name);
		continue;
	    }

	    /* deal with bad type/value combinations. */
	    if (token_type == CANCEL) {
		/*
		 * Prefer terminfo in this (long-obsolete) ambiguity:
		 */
		if (!strcmp("ma", _nc_curr_token.tk_name)) {
		    entry_ptr = _nc_find_type_entry("ma", NUMBER,
						    _nc_syntax != 0);
		    assert(entry_ptr != 0);
		}
	    } else if (entry_ptr->nte_type != token_type) {
		/*
		 * Nasty special cases here handle situations in which type
		 * information can resolve name clashes.  Normal lookup
		 * finds the last instance in the capability table of a
		 * given name, regardless of type.  find_type_entry looks
		 * for a first matching instance with given type.  So as
		 * long as all ambiguous names occur in pairs of distinct
		 * type, this will do the job.
		 */

		if (token_type == NUMBER
		    && !strcmp("ma", _nc_curr_token.tk_name)) {
		    /* tell max_attributes from arrow_key_map */
		    entry_ptr = _nc_find_type_entry("ma", NUMBER,
						    _nc_syntax != 0);
		    assert(entry_ptr != 0);

		} else if (token_type == STRING
			   && !strcmp("MT", _nc_curr_token.tk_name)) {
		    /* map terminfo's string MT to MT */
		    entry_ptr = _nc_find_type_entry("MT", STRING,
						    _nc_syntax != 0);
		    assert(entry_ptr != 0);

		} else if (token_type == BOOLEAN
			   && entry_ptr->nte_type == STRING) {
		    /* treat strings without following "=" as empty strings */
		    token_type = STRING;
		} else {
		    /* we couldn't recover; skip this token */
		    if (!silent) {
			const char *type_name;
			switch (entry_ptr->nte_type) {
			case BOOLEAN:
			    type_name = "boolean";
			    break;
			case STRING:
			    type_name = "string";
			    break;
			case NUMBER:
			    type_name = "numeric";
			    break;
			default:
			    type_name = "unknown";
			    break;
			}
			_nc_warning("wrong type used for %s capability '%s'",
				    type_name, _nc_curr_token.tk_name);
		    }
		    continue;
		}
	    }

	    /* now we know that the type/value combination is OK */
	    switch (token_type) {
	    case CANCEL:
		switch (entry_ptr->nte_type) {
		case BOOLEAN:
		    entryp->tterm.Booleans[entry_ptr->nte_index] = CANCELLED_BOOLEAN;
		    break;

		case NUMBER:
		    entryp->tterm.Numbers[entry_ptr->nte_index] = CANCELLED_NUMERIC;
		    break;

		case STRING:
		    entryp->tterm.Strings[entry_ptr->nte_index] = CANCELLED_STRING;
		    break;
		}
		break;

	    case BOOLEAN:
		entryp->tterm.Booleans[entry_ptr->nte_index] = TRUE;
		break;

	    case NUMBER:
#if !NCURSES_EXT_NUMBERS
		if (_nc_curr_token.tk_valnumber > MAX_NUMBER) {
		    entryp->tterm.Numbers[entry_ptr->nte_index] = MAX_NUMBER;
		} else
#endif
		{
		    entryp->tterm.Numbers[entry_ptr->nte_index] =
			(NCURSES_INT2) _nc_curr_token.tk_valnumber;
		}
		break;

	    case STRING:
		ptr = _nc_curr_token.tk_valstring;
		if (_nc_syntax == SYN_TERMCAP)
		    ptr = _nc_captoinfo(_nc_curr_token.tk_name,
					ptr,
					parametrized[entry_ptr->nte_index]);
		entryp->tterm.Strings[entry_ptr->nte_index] = _nc_save_str(ptr);
		break;

	    default:
		if (!silent)
		    _nc_warning("unknown token type");
		_nc_panic_mode((char) ((_nc_syntax == SYN_TERMCAP) ? ':' : ','));
		continue;
	    }
	}			/* end else cur_token.name != "use" */
      nexttok:
	continue;		/* cannot have a label w/o statement */
    }				/* endwhile (not EOF and not NAMES) */

    _nc_push_token(token_type);
    _nc_set_type(_nc_first_name(entryp->tterm.term_names));

    /*
     * Try to deduce as much as possible from extension capabilities
     * (this includes obsolete BSD capabilities).  Sigh...it would be more
     * space-efficient to call this after use resolution, but it has
     * to be done before entry allocation is wrapped up.
     */
    if (!literal) {
	if (_nc_syntax == SYN_TERMCAP) {
	    bool has_base_entry = FALSE;

	    /*
	     * Don't insert defaults if this is a `+' entry meant only
	     * for inclusion in other entries (not sure termcap ever
	     * had these, actually).
	     */
	    if (strchr(entryp->tterm.term_names, '+')) {
		has_base_entry = TRUE;
	    } else {
		unsigned i;
		/*
		 * Otherwise, look for a base entry that will already
		 * have picked up defaults via translation.
		 */
		for (i = 0; i < entryp->nuses; i++) {
		    if (entryp->uses[i].name != 0
			&& !strchr(entryp->uses[i].name, '+'))
			has_base_entry = TRUE;
		}
	    }

	    postprocess_termcap(&entryp->tterm, has_base_entry);
	} else
	    postprocess_terminfo(&entryp->tterm);
    }
    _nc_wrap_entry(entryp, FALSE);

    return (OK);
}