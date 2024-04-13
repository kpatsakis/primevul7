usm_lookup_alg_type(const char *str, usm_alg_type_t *types)
{
    int i, l;
    l = strlen(str);
    for (i = 0; types[i].label; ++i) {
        if (0 == strncasecmp(types[i].label, str, l))
            return types[i].value;
    }

    return -1;
}