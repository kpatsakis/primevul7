gs_errorname(i_ctx_t *i_ctx_p, int code, ref * perror_name)
{
    ref *perrordict, *pErrorNames;

    if (dict_find_string(systemdict, "errordict", &perrordict) <= 0 ||
        dict_find_string(systemdict, "ErrorNames", &pErrorNames) <= 0
        )
        return_error(gs_error_undefined);      /* errordict or ErrorNames not found?! */
    return array_get(imemory, pErrorNames, (long)(-code - 1), perror_name);
}