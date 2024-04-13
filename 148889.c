fstring_parse(const char **str, const char *end, int raw, int recurse_lvl,
              struct compiling *c, const node *n)
{
    FstringParser state;

    FstringParser_Init(&state);
    if (FstringParser_ConcatFstring(&state, str, end, raw, recurse_lvl,
                                    c, n) < 0) {
        FstringParser_Dealloc(&state);
        return NULL;
    }

    return FstringParser_Finish(&state, c, n);
}