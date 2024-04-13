free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    for (p = buf->bh_first.b_next; p != NULL; p = np)
    {
	np = p->b_next;
	vim_free(p);
    }
    buf->bh_first.b_next = NULL;
}