static Token *tokenize(const char *line)
{
    enum pp_token_type type;
    Token *list = NULL;
    Token *t, **tail = &list;

    while (*line) {
        const char *p = line;
        const char *ep = NULL;  /* End of token, for trimming the end */
        size_t toklen;
        char firstchar = *p;    /* Can be used to override the first char */

        if (*p == '%') {
            /*
             * Preprocessor construct; find the end of the token.
             * Classification is handled later, because %{...} can be
             * used to create any preprocessor token.
             */
            p++;
            if (*p == '+' && !nasm_isdigit(p[1])) {
                /* Paste token */
                p++;
            } else if (nasm_isdigit(*p) ||
                       ((*p == '-' || *p == '+') && nasm_isdigit(p[1]))) {
                do {
                    p++;
                }
                while (nasm_isdigit(*p));
            } else if (*p == '{' || *p == '[') {
                /* %{...} or %[...] */
                char firstchar = *p;
                char endchar = *p + 2; /* } or ] */
                int lvl = 1;
                line += (*p++ == '{'); /* Skip { but not [ (yet) */
                while (lvl) {
                    if (*p == firstchar) {
                        lvl++;
                    } else if (*p == endchar) {
                        lvl--;
                    } else if (nasm_isquote(*p)) {
                        p = nasm_skip_string(p);
                    }

                    /*
                     * *p can have been advanced to a null character by
                     * nasm_skip_string()
                     */
                    if (!*p) {
                        nasm_warn(WARN_OTHER, "unterminated %%%c construct",
                                  firstchar);
                        break;
                    }
                    p++;
                }
                ep = lvl ? p : p-1; /* Terminal character not part of token */
            } else if (*p == '?') {
                /* %? or %?? */
                p++;
                if (*p == '?')
                    p++;
            } else if (*p == '*' && p[1] == '?') {
                /* %*? and %*?? */
                p += 2;
                if (*p == '?')
                    p++;
            } else if (*p == '!') {
                /* Environment variable reference */
                p++;
                if (nasm_isidchar(*p)) {
                    do {
                        p++;
                    }
                    while (nasm_isidchar(*p));
                } else if (nasm_isquote(*p)) {
                    p = nasm_skip_string(p);
                    if (*p)
                        p++;
                    else
                        nasm_nonfatalf(ERR_PASS1, "unterminated %%! string");
                } else {
                    /* %! without anything else... */
                }
            } else if (*p == ',') {
                /* Conditional comma */
                p++;
            } else if (nasm_isidchar(*p) ||
                       ((*p == '%' || *p == '$') && nasm_isidchar(p[1]))) {
                /* Identifier or some sort */
                do {
                    p++;
                }
                while (nasm_isidchar(*p));
            } else if (*p == '%') {
                /* %% operator */
                p++;
            }

            if (!ep)
                ep = p;
            toklen = ep - line;

            /* Classify here, to handle %{...} correctly */
            if (toklen < 2) {
                type = TOK_OTHER;   /* % operator */
            } else {
                char c0 = line[1];

                switch (c0) {
                case '+':
                    type = (toklen == 2) ? TOK_PASTE : TOK_MMACRO_PARAM;
                    break;

                case '-':
                    type = TOK_MMACRO_PARAM;
                    break;

                case '?':
                    if (toklen == 2)
                        type = TOK_PREPROC_Q;
                    else if (toklen == 3 && line[2] == '?')
                        type = TOK_PREPROC_QQ;
                    else
                        type = TOK_PREPROC_ID;
                    break;

                case '*':
                    type = TOK_OTHER;
                    if (line[2] == '?') {
                        if (toklen == 3)
                            type = TOK_PREPROC_SQ;
                        else if (toklen == 4 && line[3] == '?')
                            type = TOK_PREPROC_SQQ;
                    }
                    break;

                case '!':
                    type = (toklen == 2) ? TOK_OTHER : TOK_ENVIRON;
                    break;

                case '%':
                    type = (toklen == 2) ? TOK_OTHER : TOK_LOCAL_SYMBOL;
                    break;

                case '$':
                    type = (toklen == 2) ? TOK_OTHER : TOK_LOCAL_MACRO;
                    break;

                case '[':
                    line += 2;  /* Skip %[ */
                    firstchar = *line; /* Don't clobber */
                    toklen -= 2;
                    type = TOK_INDIRECT;
                    break;

                case ',':
                    type = (toklen == 2) ? TOK_COND_COMMA : TOK_PREPROC_ID;
                    break;

                case '\'':
                case '\"':
                case '`':
                    /* %{'string'} */
                    type = TOK_PREPROC_ID;
                    break;

                case ':':
                    type = TOK_MMACRO_PARAM; /* %{:..} */
                    break;

                default:
                    if (nasm_isdigit(c0))
                        type = TOK_MMACRO_PARAM;
                    else if (nasm_isidchar(c0) || toklen > 2)
                        type = TOK_PREPROC_ID;
                    else
                        type = TOK_OTHER;
                    break;
                }
            }
        } else if (nasm_isidstart(*p) || (*p == '$' && nasm_isidstart(p[1]))) {
            /*
             * An identifier. This includes the ? operator, which is
             * treated as a keyword, not as a special character
             * operator
             */
            type = TOK_ID;
            while (nasm_isidchar(*++p))
                ;
         } else if (nasm_isquote(*p)) {
            /*
             * A string token.
             */
            type = TOK_STRING;
            p = nasm_skip_string(p);

            if (*p) {
                p++;
            } else {
                nasm_warn(WARN_OTHER, "unterminated string");
                /* Handling unterminated strings by UNV */
                /* type = -1; */
            }
        } else if (p[0] == '$' && p[1] == '$') {
            /* TOKEN_BASE - treat as TOK_ID for pasting purposes */
            type = TOK_ID;
            p += 2;
        } else if (nasm_isnumstart(*p)) {
            bool is_hex = false;
            bool is_float = false;
            bool has_e = false;
            char c;

            /*
             * A numeric token.
             */

            if (*p == '$') {
                p++;
                is_hex = true;
            }

            for (;;) {
                c = *p++;

                if (!is_hex && (c == 'e' || c == 'E')) {
                    has_e = true;
                    if (*p == '+' || *p == '-') {
                        /*
                         * e can only be followed by +/- if it is either a
                         * prefixed hex number or a floating-point number
                         */
                        p++;
                        is_float = true;
                    }
                } else if (c == 'H' || c == 'h' || c == 'X' || c == 'x') {
                    is_hex = true;
                } else if (c == 'P' || c == 'p') {
                    is_float = true;
                    if (*p == '+' || *p == '-')
                        p++;
                } else if (nasm_isnumchar(c))
                    ; /* just advance */
                else if (c == '.') {
                    /*
                     * we need to deal with consequences of the legacy
                     * parser, like "1.nolist" being two tokens
                     * (TOK_NUMBER, TOK_ID) here; at least give it
                     * a shot for now.  In the future, we probably need
                     * a flex-based scanner with proper pattern matching
                     * to do it as well as it can be done.  Nothing in
                     * the world is going to help the person who wants
                     * 0x123.p16 interpreted as two tokens, though.
                     */
                    const char *r = p;
                    while (*r == '_')
                        r++;

                    if (nasm_isdigit(*r) || (is_hex && nasm_isxdigit(*r)) ||
                        (!is_hex && (*r == 'e' || *r == 'E')) ||
                        (*r == 'p' || *r == 'P')) {
                        p = r;
                        is_float = true;
                    } else
                        break;  /* Terminate the token */
                } else
                    break;
            }
            p--;        /* Point to first character beyond number */

            if (p == line+1 && *line == '$') {
                /* TOKEN_HERE - treat as TOK_ID for pasting purposes */
                type = TOK_ID;
            } else {
                if (has_e && !is_hex) {
                    /* 1e13 is floating-point, but 1e13h is not */
                    is_float = true;
                }

                type = is_float ? TOK_FLOAT : TOK_NUMBER;
            }
        } else if (nasm_isspace(*p)) {
            type = TOK_WHITESPACE;
            p = nasm_skip_spaces(p);
            /*
             * Whitespace just before end-of-line is discarded by
             * pretending it's a comment; whitespace just before a
             * comment gets lumped into the comment.
             */
            if (!*p || *p == ';') {
                type = TOK_COMMENT;
                while (*p)
                    p++;
            }
        } else if (*p == ';') {
            type = TOK_COMMENT;
            while (*p)
                p++;
        } else {
            /*
             * Anything else is an operator of some kind. We check
             * for all the double-character operators (>>, <<, //,
             * %%, <=, >=, ==, !=, <>, &&, ||, ^^) and the triple-
	     * character operators (<<<, >>>, <=>) but anything
             * else is a single-character operator.
             */
            type = TOK_OTHER;
	    switch (*p++) {
	    case '>':
		if (*p == '>') {
		    p++;
		    if (*p == '>')
			p++;
		} else if (*p == '=') {
                    p++;
                }
		break;

	    case '<':
		if (*p == '<') {
		    p++;
		    if (*p == '<')
			p++;
		} else if (*p == '=') {
		    p++;
		    if (*p == '>')
			p++;
		} else if (*p == '>') {
		    p++;
		}
		break;

	    case '!':
		if (*p == '=')
		    p++;
		break;

	    case '/':
	    case '=':
	    case '&':
	    case '|':
	    case '^':
		/* These operators can be doubled but nothing else */
		if (*p == p[-1])
		    p++;
		break;

	    default:
		break;
	    }
        }

        if (type == TOK_WHITESPACE) {
            *tail = t = new_White(NULL);
            tail = &t->next;
        } else if (type != TOK_COMMENT) {
            if (!ep)
                ep = p;
            *tail = t = new_Token(NULL, type, line, ep - line);
            *tok_text_buf(t) = firstchar; /* E.g. %{foo} -> {foo -> %foo */
            tail = &t->next;
        }
        line = p;
    }
    return list;
}