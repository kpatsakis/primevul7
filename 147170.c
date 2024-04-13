putHmarker(HmarkerList *ml, int line, int pos, int seq)
{
    if (ml == NULL) {
	ml = New(HmarkerList);
	ml->marks = NULL;
	ml->nmark = 0;
	ml->markmax = 0;
	ml->prevhseq = -1;
    }
    if (ml->markmax == 0) {
	ml->markmax = FIRST_MARKER_SIZE;
	ml->marks = NewAtom_N(BufferPoint, ml->markmax);
	bzero(ml->marks, sizeof(BufferPoint) * ml->markmax);
    }
    if (seq + 1 > ml->nmark)
	ml->nmark = seq + 1;
    if (ml->nmark >= ml->markmax) {
	ml->markmax = ml->nmark * 2;
	ml->marks = New_Reuse(BufferPoint, ml->marks, ml->markmax);
    }
    ml->marks[seq].line = line;
    ml->marks[seq].pos = pos;
    ml->marks[seq].invalid = 0;
    return ml;
}