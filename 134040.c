static Token *steal_Token(Token *dst, Token *src)
{
    /* Overwrite everything except the next pointers */
    memcpy((char *)dst + sizeof(Token *), (char *)src + sizeof(Token *),
	   sizeof(Token) - sizeof(Token *));

    /* Clear the donor token */
    memset((char *)src + sizeof(Token *), 0, sizeof(Token) - sizeof(Token *));

    return dst;
}