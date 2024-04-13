putAnchor(AnchorList *al, char *url, char *target, Anchor **anchor_return,
	  char *referer, char *title, unsigned char key, int line, int pos)
{
    int n, i, j;
    Anchor *a;
    BufferPoint bp = { 0 };
    if (al == NULL) {
	al = New(AnchorList);
	al->anchors = NULL;
	al->nanchor = al->anchormax = 0;
	al->acache = -1;
    }
    if (al->anchormax == 0) {
	/* first time; allocate anchor buffer */
	al->anchors = New_N(Anchor, FIRST_ANCHOR_SIZE);
	al->anchormax = FIRST_ANCHOR_SIZE;
    }
    if (al->nanchor == al->anchormax) {	/* need realloc */
	al->anchormax *= 2;
	al->anchors = New_Reuse(Anchor, al->anchors, al->anchormax);
    }
    bp.line = line;
    bp.pos = pos;
    n = al->nanchor;
    if (!n || bpcmp(al->anchors[n - 1].start, bp) < 0)
	i = n;
    else
	for (i = 0; i < n; i++) {
	    if (bpcmp(al->anchors[i].start, bp) >= 0) {
		for (j = n; j > i; j--)
		    al->anchors[j] = al->anchors[j - 1];
		break;
	    }
	}
    a = &al->anchors[i];
    a->url = url;
    a->target = target;
    a->referer = referer;
    a->title = title;
    a->accesskey = key;
    a->slave = FALSE;
    a->start = bp;
    a->end = bp;
    al->nanchor++;
    if (anchor_return)
	*anchor_return = a;
    return al;
}