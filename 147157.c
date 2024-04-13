searchAnchor(AnchorList *al, char *str)
{
    int i;
    Anchor *a;
    if (al == NULL)
	return NULL;
    for (i = 0; i < al->nanchor; i++) {
	a = &al->anchors[i];
	if (a->hseq < 0)
	  continue;
	if (!strcmp(a->url, str))
	    return a;
    }
    return NULL;
}