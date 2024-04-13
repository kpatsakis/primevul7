find_cmdline_var(char_u *src, int *usedlen)
{
    int		len;
    int		i;
    static char *(spec_str[]) = {
		    "%",
#define SPEC_PERC   0
		    "#",
#define SPEC_HASH   (SPEC_PERC + 1)
		    "<cword>",		// cursor word
#define SPEC_CWORD  (SPEC_HASH + 1)
		    "<cWORD>",		// cursor WORD
#define SPEC_CCWORD (SPEC_CWORD + 1)
		    "<cexpr>",		// expr under cursor
#define SPEC_CEXPR  (SPEC_CCWORD + 1)
		    "<cfile>",		// cursor path name
#define SPEC_CFILE  (SPEC_CEXPR + 1)
		    "<sfile>",		// ":so" file name
#define SPEC_SFILE  (SPEC_CFILE + 1)
		    "<slnum>",		// ":so" file line number
#define SPEC_SLNUM  (SPEC_SFILE + 1)
		    "<stack>",		// call stack
#define SPEC_STACK  (SPEC_SLNUM + 1)
		    "<script>",		// script file name
#define SPEC_SCRIPT (SPEC_STACK + 1)
		    "<afile>",		// autocommand file name
#define SPEC_AFILE  (SPEC_SCRIPT + 1)
		    "<abuf>",		// autocommand buffer number
#define SPEC_ABUF   (SPEC_AFILE + 1)
		    "<amatch>",		// autocommand match name
#define SPEC_AMATCH (SPEC_ABUF + 1)
		    "<sflnum>",		// script file line number
#define SPEC_SFLNUM  (SPEC_AMATCH + 1)
		    "<SID>",		// script ID: <SNR>123_
#define SPEC_SID  (SPEC_SFLNUM + 1)
#ifdef FEAT_CLIENTSERVER
		    "<client>"
# define SPEC_CLIENT (SPEC_SID + 1)
#endif
    };

    for (i = 0; i < (int)ARRAY_LENGTH(spec_str); ++i)
    {
	len = (int)STRLEN(spec_str[i]);
	if (STRNCMP(src, spec_str[i], len) == 0)
	{
	    *usedlen = len;
	    return i;
	}
    }
    return -1;
}