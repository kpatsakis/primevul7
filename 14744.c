script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = NULL;

    for (;;)
    {
	char_u *n = (char_u *)strstr((char *)p, "autoload");

	if (n == NULL)
	    break;
	if (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    return res;
}