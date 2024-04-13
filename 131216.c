spell_enc(void)
{

    if (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	return p_enc;
    return (char_u *)"latin1";
}