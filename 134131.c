get_use_pkg(Token *t, const char *dname, const char **name)
{
    const char *id;

    t = skip_white(t);
    t = expand_smacro(t);

    *name = NULL;

    if (!t) {
        nasm_nonfatal("`%s' expects a package name, got end of line", dname);
        return NULL;
    } else if (t->type != TOK_ID && t->type != TOK_STRING) {
        nasm_nonfatal("`%s' expects a package name, got `%s'",
                      dname, tok_text(t));
        return NULL;
    }

    *name = id = unquote_token(t);

    t = t->next;
    t = skip_white(t);
    if (t)
        nasm_warn(WARN_OTHER, "trailing garbage after `%s' ignored", dname);

    return nasm_find_use_package(id);
}