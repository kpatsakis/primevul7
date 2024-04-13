GetComponentSpec(unsigned char **pWire, Bool allowExpr, int *errRtrn)
{
    int len;
    register int i;
    unsigned char *wire, *str, *tmp, *legal;

    if (allowExpr)
        legal = &componentExprLegal[0];
    else
        legal = &componentSpecLegal[0];

    wire = *pWire;
    len = (*(unsigned char *) wire++);
    if (len > 0) {
        str = calloc(1, len + 1);
        if (str) {
            tmp = str;
            for (i = 0; i < len; i++) {
                if (legal[(*wire) / 8] & (1 << ((*wire) % 8)))
                    *tmp++ = *wire++;
                else
                    wire++;
            }
            if (tmp != str)
                *tmp++ = '\0';
            else {
                free(str);
                str = NULL;
            }
        }
        else {
            *errRtrn = BadAlloc;
        }
    }
    else {
        str = NULL;
    }
    *pWire = wire;
    return (char *) str;
}