push_callout(i_ctx_t *i_ctx_p, const char *callout_name)
{
    int code;

    check_estack(1);
    code = name_enter_string(imemory, callout_name, esp + 1);
    if (code < 0)
        return code;
    ++esp;
    r_set_attrs(esp, a_executable);
    return o_push_estack;
}