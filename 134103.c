static void pp_pre_define(char *definition)
{
    Token *def, *space;
    Line *l;
    char *equals;

    equals = strchr(definition, '=');
    space = new_White(NULL);
    def = new_Token(space, TOK_PREPROC_ID, "%define", 0);
    if (equals)
        *equals = ' ';
    space->next = tokenize(definition);
    if (equals)
        *equals = '=';

    /* We can't predefine a TOK_LOCAL_MACRO for obvious reasons... */
    if (space->next->type != TOK_PREPROC_ID &&
        space->next->type != TOK_ID)
        nasm_warn(WARN_OTHER, "pre-defining non ID `%s\'\n", definition);

    l = nasm_malloc(sizeof(Line));
    l->next = predef;
    l->first = def;
    l->finishes = NULL;
    predef = l;
}