gs_errorinfo_put_string(i_ctx_t *i_ctx_p, const char *str)
{
    ref rstr;
    ref *pderror;
    int code = string_to_ref(str, &rstr, iimemory, "gs_errorinfo_put_string");

    if (code < 0)
        return code;
    if (dict_find_string(systemdict, "$error", &pderror) <= 0 ||
        !r_has_type(pderror, t_dictionary) ||
        idict_put_string(pderror, "errorinfo", &rstr) < 0
        )
        return_error(gs_error_Fatal);
    return 0;
}