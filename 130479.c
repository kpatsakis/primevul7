indent_DYN(DYNBUF * buffer, int level)
{
    int n;

    for (n = 0; n < level; n++)
	strncpy_DYN(buffer, "\t", (size_t) 1);
}