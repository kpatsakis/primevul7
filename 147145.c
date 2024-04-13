closest_prev_anchor(AnchorList *a, Anchor *an, int x, int y)
{
    int i;

    if (a == NULL || a->nanchor == 0)
	return an;
    for (i = 0; i < a->nanchor; i++) {
	if (a->anchors[i].hseq < 0)
	    continue;
	if (a->anchors[i].end.line < y ||
	    (a->anchors[i].end.line == y && a->anchors[i].end.pos <= x)) {
	    if (an == NULL || an->end.line < a->anchors[i].end.line ||
		(an->end.line == a->anchors[i].end.line &&
		 an->end.pos < a->anchors[i].end.pos))
		an = &a->anchors[i];
	}
    }
    return an;
}