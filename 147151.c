retrieveAnchor(AnchorList *al, int line, int pos)
{
    Anchor *a;
    size_t b, e;
    int cmp;

    if (al == NULL || al->nanchor == 0)
	return NULL;

    if (al->acache < 0 || al->acache >= al->nanchor)
	al->acache = 0;

    for (b = 0, e = al->nanchor - 1; b <= e; al->acache = (b + e) / 2) {
	a = &al->anchors[al->acache];
	cmp = onAnchor(a, line, pos);
	if (cmp == 0)
	    return a;
	else if (cmp > 0)
	    b = al->acache + 1;
	else if (al->acache == 0)
	    return NULL;
	else
	    e = al->acache - 1;
    }
    return NULL;
}