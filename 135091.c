void GfxIndexedColorSpace::getCMYKLine(unsigned char *in, unsigned char *out, int length)
{
    unsigned char *line;
    int i, j, n;

    n = base->getNComps();
    line = (unsigned char *)gmallocn(length, n);
    for (i = 0; i < length; i++)
        for (j = 0; j < n; j++)
            line[i * n + j] = lookup[in[i] * n + j];

    base->getCMYKLine(line, out, length);

    gfree(line);
}