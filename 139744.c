DU_stripLeadingSpaces(char *s)
{
    int i, j, n;

    if (s == NULL) return;
    n = OFstatic_cast(int, strlen(s));
    if (n == 0) return;
    if (!isspace(TO_UCHAR(s[0]))) return; /* no leading space */

    /* first non-space */
    for (i=0; i<n && isspace(TO_UCHAR(s[i])); i++)
        /* do nothing, just iterate */
        ;
    if (i<n) {
        /* found non-space, pull to front (inclusive '\0') */
        for (j=i; j<=n; j++) {
            s[j-i] = s[j];
        }
    } else {
        /* all spaces */
        s[0] = '\0';
    }
}