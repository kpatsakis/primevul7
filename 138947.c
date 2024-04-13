read_readbuffers(int advance)
{
    int c;

    c = read_readbuf(&readbuf1, advance);
    if (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    return c;
}