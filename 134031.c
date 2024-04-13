static Token *zap_white(Token *x)
{
    while (tok_white(x))
        x = delete_Token(x);

    return x;
}