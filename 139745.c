DU_stripTrailingSpaces(char *s)
{
    int i, n;

    if (s)
    {
        n = OFstatic_cast(int, strlen(s));
        for (i = n - 1; i >= 0 && isspace(TO_UCHAR(s[i])); i--)
            s[i] = '\0';
    }
}