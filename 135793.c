static int similarity_index(struct diff_filepair *p)
{
	return p->score * 100 / MAX_SCORE;
}