static size_t tok_strlen(const char *str)
{
    return strnlen(str, MAX_TEXT+1);
}