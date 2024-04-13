reAnchorPos(Buffer *buf, Line *l, char *p1, char *p2,
	    Anchor *(*anchorproc) (Buffer *, char *, char *, int, int))
{
    Anchor *a;
    int spos, epos;
    int i, hseq = -2;

    spos = p1 - l->lineBuf;
    epos = p2 - l->lineBuf;
    for (i = spos; i < epos; i++) {
	if (l->propBuf[i] & (PE_ANCHOR | PE_FORM))
	    return p2;
    }
    for (i = spos; i < epos; i++)
	l->propBuf[i] |= PE_ANCHOR;
    while (spos > l->len && l->next && l->next->bpos) {
	spos -= l->len;
	epos -= l->len;
	l = l->next;
    }
    while (1) {
	a = anchorproc(buf, p1, p2, l->linenumber, spos);
	a->hseq = hseq;
	if (hseq == -2) {
	    reseq_anchor(buf);
	    hseq = a->hseq;
	}
	a->end.line = l->linenumber;
	if (epos > l->len && l->next && l->next->bpos) {
	    a->end.pos = l->len;
	    spos = 0;
	    epos -= l->len;
	    l = l->next;
	}
	else {
	    a->end.pos = epos;
	    break;
	}
    }
    return p2;
}