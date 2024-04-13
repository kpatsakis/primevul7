static int read_param_count(const char *str)
{
    int result;
    bool err;

    result = readnum(str, &err);
    if (result < 0 || result > INT_MAX) {
        result = 0;
        nasm_nonfatal("parameter count `%s' is out of bounds [%d; %d]",
                      str, 0, INT_MAX);
    } else if (err)
        nasm_nonfatal("unable to parse parameter count `%s'", str);
    return result;
}