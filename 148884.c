forbidden_name(struct compiling *c, identifier name, const node *n,
               int full_checks)
{
    assert(PyUnicode_Check(name));
    if (_PyUnicode_EqualToASCIIString(name, "__debug__")) {
        ast_error(c, n, "assignment to keyword");
        return 1;
    }
    if (full_checks) {
        const char * const *p;
        for (p = FORBIDDEN; *p; p++) {
            if (_PyUnicode_EqualToASCIIString(name, *p)) {
                ast_error(c, n, "assignment to keyword");
                return 1;
            }
        }
    }
    return 0;
}