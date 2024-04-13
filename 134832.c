void
Perl_pregfree(pTHX_ REGEXP *r)
{
    SvREFCNT_dec(r);