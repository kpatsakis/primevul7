vpeekc_nomap(void)
{
    int		c;

    ++no_mapping;
    ++allow_keys;
    c = vpeekc();
    --no_mapping;
    --allow_keys;
    return c;
}