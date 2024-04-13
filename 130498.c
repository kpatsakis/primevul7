wrap_concat3(const char *name, const char *eqls, const char *value)
{
    int nlen = (int) strlen(name);
    int elen = (int) strlen(eqls);
    int vlen = (int) strlen(value);

    wrap_concat(name, nlen + elen + vlen, w1ST);
    wrap_concat(eqls, elen + vlen, w2ND);
    wrap_concat(value, vlen, wEND);
}