getAnchorText(Buffer *buf, AnchorList *al, Anchor *a)
{
    int hseq, i;
    Line *l;
    Str tmp = NULL;
    char *p, *ep;

    if (!a || a->hseq < 0)
	return NULL;
    hseq = a->hseq;
    l = buf->firstLine;
    for (i = 0; i < al->nanchor; i++) {
	a = &al->anchors[i];
	if (a->hseq != hseq)
	    continue;
	for (; l; l = l->next) {
	    if (l->linenumber == a->start.line)
		break;
	}
	if (!l)
	    break;
	p = l->lineBuf + a->start.pos;
	ep = l->lineBuf + a->end.pos;
	for (; p < ep && IS_SPACE(*p); p++) ;
	if (p == ep)
	    continue;
	if (!tmp)
	    tmp = Strnew_size(ep - p);
	else
	    Strcat_char(tmp, ' ');
	Strcat_charp_n(tmp, p, ep - p);
    }
    return tmp ? tmp->ptr : NULL;
}