STATIC void
S_re_croak2(pTHX_ bool utf8, const char* pat1, const char* pat2,...)
{
    va_list args;
    STRLEN l1 = strlen(pat1);
    STRLEN l2 = strlen(pat2);
    char buf[512];
    SV *msv;
    const char *message;

    PERL_ARGS_ASSERT_RE_CROAK2;

    if (l1 > 510)
	l1 = 510;
    if (l1 + l2 > 510)
	l2 = 510 - l1;
    Copy(pat1, buf, l1 , char);
    Copy(pat2, buf + l1, l2 , char);
    buf[l1 + l2] = '\n';
    buf[l1 + l2 + 1] = '\0';
    va_start(args, pat2);
    msv = vmess(buf, &args);
    va_end(args);
    message = SvPV_const(msv, l1);
    if (l1 > 512)
	l1 = 512;
    Copy(message, buf, l1 , char);
    /* l1-1 to avoid \n */
    Perl_croak(aTHX_ "%" UTF8f, UTF8fARG(utf8, l1-1, buf));