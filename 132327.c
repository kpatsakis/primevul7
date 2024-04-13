formtype(char *typestr)
{
    int i;
    for (i = 0; _formtypetbl[i]; i++) {
	if (!strcasecmp(typestr, _formtypetbl[i]))
	    return i;
    }
    return FORM_INPUT_TEXT;
}