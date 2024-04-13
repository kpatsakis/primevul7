free_DYN(DYNBUF * p)
{
    if (p->text != 0)
	free(p->text);
    p->text = 0;
    p->size = 0;
    p->used = 0;
}