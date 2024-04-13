reseq_anchor(Buffer *buf)
{
    int i, j, n, nmark = (buf->hmarklist) ? buf->hmarklist->nmark : 0;
    short *seqmap;
    Anchor *a, *a1;
    HmarkerList *ml = NULL;

    if (!buf->href)
	return;

    n = nmark;
    for (i = 0; i < buf->href->nanchor; i++) {
	a = &buf->href->anchors[i];
	if (a->hseq == -2)
	    n++;
    }

    if (n == nmark)
	return;

    seqmap = NewAtom_N(short, n);

    for (i = 0; i < n; i++)
	seqmap[i] = i;

    n = nmark;
    for (i = 0; i < buf->href->nanchor; i++) {
	a = &buf->href->anchors[i];
	if (a->hseq == -2) {
	    a->hseq = n;
	    a1 = closest_next_anchor(buf->href, NULL, a->start.pos,
				     a->start.line);
	    a1 = closest_next_anchor(buf->formitem, a1, a->start.pos,
				     a->start.line);
	    if (a1 && a1->hseq >= 0) {
		seqmap[n] = seqmap[a1->hseq];
		for (j = a1->hseq; j < nmark; j++)
		    seqmap[j]++;
	    }
	    ml = putHmarker(ml, a->start.line, a->start.pos, seqmap[n]);
	    n++;
	}
    }

    for (i = 0; i < nmark; i++) {
	ml = putHmarker(ml, buf->hmarklist->marks[i].line,
			buf->hmarklist->marks[i].pos, seqmap[i]);
    }
    buf->hmarklist = ml;

    reseq_anchor0(buf->href, seqmap);
    reseq_anchor0(buf->formitem, seqmap);
}