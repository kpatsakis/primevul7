static bool is_macro_id(const Token *t)
{
    return tok_type(t, TOK_ID) || tok_type(t, TOK_LOCAL_MACRO);
}