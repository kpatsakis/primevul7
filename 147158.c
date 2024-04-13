reseq_anchor0(AnchorList *al, short *seqmap)
{
    int i;
    Anchor *a;

    if (!al)
	return;

    for (i = 0; i < al->nanchor; i++) {
	a = &al->anchors[i];
	if (a->hseq >= 0) {
	    a->hseq = seqmap[a->hseq];
	}
    }
}