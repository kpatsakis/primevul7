schemeNumToName(int scheme)
{
    int i;

    for (i = 0; schemetable[i].cmdname != NULL; i++) {
	if (schemetable[i].cmd == scheme)
	    return schemetable[i].cmdname;
    }
    return NULL;
}