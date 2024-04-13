unixsock_path_value(VALUE path)
{
#ifdef __linux__
#define TO_STR_FOR_LINUX_ABSTRACT_NAMESPACE 0

    VALUE name = path;
#if TO_STR_FOR_LINUX_ABSTRACT_NAMESPACE
    const int isstr = !NIL_P(name = rb_check_string_type(name));
#else
    const int isstr = RB_TYPE_P(name, T_STRING);
#endif
    if (isstr) {
        if (RSTRING_LEN(name) == 0 || RSTRING_PTR(name)[0] == '\0') {
            rb_check_safe_obj(name);
            return name;             /* ignore encoding */
        }
    }
#endif
    return rb_get_path(path);
}