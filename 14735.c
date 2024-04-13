ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    char_u  *p = skiptowhite(arg);
    int	    len = (int)(p - arg);
    int	    flags = eap->forceit ? DIP_ALL : 0;

    if (STRNCMP(arg, "START", len) == 0)
    {
	flags += DIP_START + DIP_NORTP;
	arg = skipwhite(arg + len);
    }
    else if (STRNCMP(arg, "OPT", len) == 0)
    {
	flags += DIP_OPT + DIP_NORTP;
	arg = skipwhite(arg + len);
    }
    else if (STRNCMP(arg, "PACK", len) == 0)
    {
	flags += DIP_START + DIP_OPT + DIP_NORTP;
	arg = skipwhite(arg + len);
    }
    else if (STRNCMP(arg, "ALL", len) == 0)
    {
	flags += DIP_START + DIP_OPT;
	arg = skipwhite(arg + len);
    }

    source_runtime(arg, flags);
}