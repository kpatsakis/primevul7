wrap_concat(const char *src, int need, unsigned mode)
{
    int gaps = (int) strlen(separator);
    int want = gaps + need;

    did_wrap = (width <= 0);
    if (wrap_1ST(mode)
	&& column > indent
	&& column + want > width) {
	force_wrap();
    }
    if ((wrap_END(mode) && !wrap_ERR(mode)) &&
	wrapped &&
	(width >= 0) &&
	(column + want) > width) {
	int step = 0;
	int used = width > WRAPPED ? width : WRAPPED;
	int size;
	int base = 0;
	char *p, align[9];
	const char *my_t = trailer;
	char *fill = fill_spaces(src);
	int last = (int) strlen(fill);

	need = last;

	if (TcOutput())
	    trailer = "\\\n\t ";

	if (!TcOutput() && (p = strchr(fill, '=')) != 0) {
	    base = (int) (p + 1 - fill);
	    if (base > 8)
		base = 8;
	    _nc_SPRINTF(align, _nc_SLIMIT(align) "%*s", base, " ");
	} else if (column > 8) {
	    base = column - 8;
	    if (base > 8)
		base = 8;
	    _nc_SPRINTF(align, _nc_SLIMIT(align) "%*s", base, " ");
	} else {
	    align[base] = '\0';
	}
	/* "pretty" overrides wrapping if it already split the line */
	if (!pretty || strchr(fill, '\n') == 0) {
	    int tag = 0;

	    if (TcOutput() && outbuf.used && !wrap_1ST(mode)) {
		tag = 3;
	    }

	    while ((column + (need + gaps)) > used) {
		size = used - tag;
		if (step) {
		    strcpy_DYN(&outbuf, align);
		    size -= base;
		}
		if (size > (last - step)) {
		    size = (last - step);
		}
		size = find_split(fill, step, size);
		strncpy_DYN(&outbuf, fill + step, (size_t) size);
		step += size;
		need -= size;
		if (need > 0) {
		    force_wrap();
		    did_wrap = TRUE;
		    tag = 0;
		}
	    }
	}
	if (need > 0) {
	    if (step)
		strcpy_DYN(&outbuf, align);
	    strcpy_DYN(&outbuf, fill + step);
	}
	if (wrap_END(mode))
	    strcpy_DYN(&outbuf, separator);
	trailer = my_t;
	force_wrap();

	free(fill);
    } else {
	strcpy_DYN(&outbuf, src);
	if (wrap_END(mode))
	    strcpy_DYN(&outbuf, separator);
	column += (int) strlen(src);
    }
}