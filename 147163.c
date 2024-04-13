closest_next_anchor(AnchorList *a, Anchor *an, int x, int y)
{
    int i;

    if (a == NULL || a->nanchor == 0)
	return an;
    for (i = 0; i < a->nanchor; i++) {
	if (a->anchors[i].hseq < 0)
	    continue;
	if (a->anchors[i].start.line > y ||
	    (a->anchors[i].start.line == y && a->anchors[i].start.pos > x)) {
	    if (an == NULL || an->start.line > a->anchors[i].start.line ||
		(an->start.line == a->anchors[i].start.line &&
		 an->start.pos > a->anchors[i].start.pos))
		an = &a->anchors[i];
	}
    }
    return an;
}