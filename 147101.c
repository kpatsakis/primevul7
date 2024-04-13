copyPath(char *orgpath, int length, int option)
{
    Str tmp = Strnew();
    char ch;
    while ((ch = *orgpath) != 0 && length != 0) {
	if (option & COPYPATH_LOWERCASE)
	    ch = TOLOWER(ch);
	if (IS_SPACE(ch)) {
	    switch (option & COPYPATH_SPC_MASK) {
	    case COPYPATH_SPC_ALLOW:
		Strcat_char(tmp, ch);
		break;
	    case COPYPATH_SPC_IGNORE:
		/* do nothing */
		break;
	    case COPYPATH_SPC_REPLACE:
		Strcat_charp(tmp, "%20");
		break;
	    }
	}
	else
	    Strcat_char(tmp, ch);
	orgpath++;
	length--;
    }
    return tmp->ptr;
}