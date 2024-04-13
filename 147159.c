onAnchor(Anchor *a, int line, int pos)
{
    BufferPoint bp;
    bp.line = line;
    bp.pos = pos;

    if (bpcmp(bp, a->start) < 0)
	return -1;
    if (bpcmp(a->end, bp) <= 0)
	return 1;
    return 0;
}