coll_get_char(void)
{
    long	nr = -1;

    switch (*regparse++)
    {
	case 'd': nr = getdecchrs(); break;
	case 'o': nr = getoctchrs(); break;
	case 'x': nr = gethexchrs(2); break;
	case 'u': nr = gethexchrs(4); break;
	case 'U': nr = gethexchrs(8); break;
    }
    if (nr < 0 || nr > INT_MAX)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    return nr;
}