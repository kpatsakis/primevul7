patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    for (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}