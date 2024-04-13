usm_lookup_alg_str(int value, usm_alg_type_t *types)
{
    int i;
    for (i = 0; types[i].label; ++i)
        if (value == types[i].value)
            return types[i].label;

    return NULL;
}