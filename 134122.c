stdmac_ptr(const SMacro *s, Token **params, int nparams)
{
    (void)s;
    (void)params;
    (void)nparams;

    switch (globalbits) {
    case 16:
	return new_Token(NULL, TOK_ID, "word", 4);
    case 32:
	return new_Token(NULL, TOK_ID, "dword", 5);
    case 64:
	return new_Token(NULL, TOK_ID, "qword", 5);
    default:
        panic();
    }
}