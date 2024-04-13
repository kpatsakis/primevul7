st_push(Frag_T s, Frag_T **p, Frag_T *stack_end)
{
    Frag_T *stackp = *p;

    if (stackp >= stack_end)
	return;
    *stackp = s;
    *p = *p + 1;
}