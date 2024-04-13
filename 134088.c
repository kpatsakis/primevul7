static void pp_pre_command(const char *what, char *string)
{
    char *cmd;
    Token *def, *space;
    Line *l;

    def = tokenize(string);
    if (what) {
        space = new_White(def);
        cmd = nasm_strcat(what[0] == '%' ? "" : "%", what);
        def = new_Token(space, TOK_PREPROC_ID, cmd, nasm_last_string_len());
        nasm_free(cmd);
    }

    l = nasm_malloc(sizeof(Line));
    l->next = predef;
    l->first = def;
    l->finishes = NULL;
    predef = l;
}