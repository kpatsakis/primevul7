st_pop(Frag_T **p, Frag_T *stack)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    if (stackp < stack)
	return empty;
    return **p;
}