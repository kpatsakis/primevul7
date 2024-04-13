fmt_complex(TERMTYPE2 *tterm, const char *capability, char *src, int level)
{
    bool percent = FALSE;
    bool params = has_params(src);

    while (*src != '\0') {
	switch (*src) {
	case '^':
	    percent = FALSE;
	    strncpy_DYN(&tmpbuf, src++, (size_t) 1);
	    break;
	case '\\':
	    percent = FALSE;
	    strncpy_DYN(&tmpbuf, src++, (size_t) 1);
	    break;
	case '%':
	    percent = TRUE;
	    break;
	case '?':		/* "if" */
	case 't':		/* "then" */
	case 'e':		/* "else" */
	    if (percent) {
		percent = FALSE;
		tmpbuf.text[tmpbuf.used - 1] = '\n';
		/* treat a "%e" as else-if, on the same level */
		if (*src == 'e') {
		    indent_DYN(&tmpbuf, level);
		    strncpy_DYN(&tmpbuf, "%", (size_t) 1);
		    strncpy_DYN(&tmpbuf, src, (size_t) 1);
		    src++;
		    params = has_params(src);
		    if (!params && *src != '\0' && *src != '%') {
			strncpy_DYN(&tmpbuf, "\n", (size_t) 1);
			indent_DYN(&tmpbuf, level + 1);
		    }
		} else {
		    indent_DYN(&tmpbuf, level + 1);
		    strncpy_DYN(&tmpbuf, "%", (size_t) 1);
		    strncpy_DYN(&tmpbuf, src, (size_t) 1);
		    if (*src++ == '?') {
			src = fmt_complex(tterm, capability, src, level + 1);
			if (*src != '\0' && *src != '%') {
			    strncpy_DYN(&tmpbuf, "\n", (size_t) 1);
			    indent_DYN(&tmpbuf, level + 1);
			}
		    } else if (level == 1) {
			if (checking)
			    _nc_warning("%s: %%%c without %%? in %s",
					_nc_first_name(tterm->term_names),
					*src, capability);
		    }
		}
		continue;
	    }
	    break;
	case ';':		/* "endif" */
	    if (percent) {
		percent = FALSE;
		if (level > 1) {
		    tmpbuf.text[tmpbuf.used - 1] = '\n';
		    indent_DYN(&tmpbuf, level);
		    strncpy_DYN(&tmpbuf, "%", (size_t) 1);
		    strncpy_DYN(&tmpbuf, src++, (size_t) 1);
		    if (src[0] == '%'
			&& src[1] != '\0'
			&& (strchr("?e;", src[1])) == 0) {
			tmpbuf.text[tmpbuf.used++] = '\n';
			indent_DYN(&tmpbuf, level);
		    }
		    return src;
		}
		if (checking)
		    _nc_warning("%s: %%; without %%? in %s",
				_nc_first_name(tterm->term_names),
				capability);
	    }
	    break;
	case 'p':
	    if (percent && params && !leading_DYN(&tmpbuf, "%")) {
		tmpbuf.text[tmpbuf.used - 1] = '\n';
		indent_DYN(&tmpbuf, level + 1);
		strncpy_DYN(&tmpbuf, "%", (size_t) 1);
	    }
	    params = FALSE;
	    percent = FALSE;
	    break;
	case ' ':
	    strncpy_DYN(&tmpbuf, "\\s", (size_t) 2);
	    ++src;
	    continue;
	default:
	    percent = FALSE;
	    break;
	}
	strncpy_DYN(&tmpbuf, src++, (size_t) 1);
    }
    return src;
}