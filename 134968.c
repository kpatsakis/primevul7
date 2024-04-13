static I32
S_backref_value(char *p, char *e)
{
    const char* endptr = e;
    UV val;
    if (grok_atoUV(p, &val, &endptr) && val <= I32_MAX)
        return (I32)val;
    return I32_MAX;