reAnchorAny(Buffer *buf, char *re,
	    Anchor *(*anchorproc) (Buffer *, char *, char *, int, int))
{
    Line *l;
    char *p = NULL, *p1, *p2;

    if (re == NULL || *re == '\0') {
	return NULL;
    }
    if ((re = regexCompile(re, 1)) != NULL) {
	return re;
    }
    for (l = MarkAllPages ? buf->firstLine : buf->topLine; l != NULL &&
	 (MarkAllPages || l->linenumber < buf->topLine->linenumber + LASTLINE);
	 l = l->next) {
	if (p && l->bpos)
	    goto next_line;
	p = l->lineBuf;
	for (;;) {
	    if (regexMatch(p, &l->lineBuf[l->size] - p, p == l->lineBuf) == 1) {
		matchedPosition(&p1, &p2);
		p = reAnchorPos(buf, l, p1, p2, anchorproc);
	    }
	    else
		break;
	}
      next_line:
	if (MarkAllPages && l->next == NULL && buf->pagerSource &&
	    !(buf->bufferprop & BP_CLOSE))
	    getNextPage(buf, PagerMax);
    }
    return NULL;
}