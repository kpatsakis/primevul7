form_update_line(Line *line, char **str, int spos, int epos, int width,
		 int newline, int password)
{
    int c_len = 1, c_width = 1, w, i, len, pos;
    char *p, *buf;
    Lineprop c_type, effect, *prop;

    for (p = *str, w = 0, pos = 0; *p && w < width;) {
	c_type = get_mctype((unsigned char *)p);
#ifdef USE_M17N
	c_len = get_mclen(p);
	c_width = get_mcwidth(p);
#endif
	if (c_type == PC_CTRL) {
	    if (newline && *p == '\n')
		break;
	    if (*p != '\r') {
		w++;
		pos++;
	    }
	}
	else if (password) {
#ifdef USE_M17N
	    if (w + c_width > width)
		break;
#endif
	    w += c_width;
	    pos += c_width;
#ifdef USE_M17N
	}
	else if (c_type & PC_UNKNOWN) {
	    w++;
	    pos++;
	}
	else {
	    if (w + c_width > width)
		break;
#endif
	    w += c_width;
	    pos += c_len;
	}
	p += c_len;
    }
    pos += width - w;

    len = line->len + pos + spos - epos;
    buf = New_N(char, len);
    prop = New_N(Lineprop, len);
    bcopy((void *)line->lineBuf, (void *)buf, spos * sizeof(char));
    bcopy((void *)line->propBuf, (void *)prop, spos * sizeof(Lineprop));

    effect = CharEffect(line->propBuf[spos]);
    for (p = *str, w = 0, pos = spos; *p && w < width;) {
	c_type = get_mctype((unsigned char *)p);
#ifdef USE_M17N
	c_len = get_mclen(p);
	c_width = get_mcwidth(p);
#endif
	if (c_type == PC_CTRL) {
	    if (newline && *p == '\n')
		break;
	    if (*p != '\r') {
		buf[pos] = password ? '*' : ' ';
		prop[pos] = effect | PC_ASCII;
		pos++;
		w++;
	    }
	}
	else if (password) {
#ifdef USE_M17N
	    if (w + c_width > width)
		break;
#endif
	    for (i = 0; i < c_width; i++) {
		buf[pos] = '*';
		prop[pos] = effect | PC_ASCII;
		pos++;
		w++;
	    }
#ifdef USE_M17N
	}
	else if (c_type & PC_UNKNOWN) {
	    buf[pos] = ' ';
	    prop[pos] = effect | PC_ASCII;
	    pos++;
	    w++;
	}
	else {
	    if (w + c_width > width)
		break;
#else
	}
	else {
#endif
	    buf[pos] = *p;
	    prop[pos] = effect | c_type;
	    pos++;
#ifdef USE_M17N
	    c_type = (c_type & ~PC_WCHAR1) | PC_WCHAR2;
	    for (i = 1; i < c_len; i++) {
		buf[pos] = p[i];
		prop[pos] = effect | c_type;
		pos++;
	    }
#endif
	    w += c_width;
	}
	p += c_len;
    }
    for (; w < width; w++) {
	buf[pos] = ' ';
	prop[pos] = effect | PC_ASCII;
	pos++;
    }
    if (newline) {
	if (!FoldTextarea) {
	    while (*p && *p != '\r' && *p != '\n')
		p++;
	}
	if (*p == '\r')
	    p++;
	if (*p == '\n')
	    p++;
    }
    *str = p;

    bcopy((void *)&line->lineBuf[epos], (void *)&buf[pos],
	  (line->len - epos) * sizeof(char));
    bcopy((void *)&line->propBuf[epos], (void *)&prop[pos],
	  (line->len - epos) * sizeof(Lineprop));
    line->lineBuf = buf;
    line->propBuf = prop;
    line->len = len;
    line->size = len;

    return pos;
}