PERL_STATIC_INLINE const char *
S_get_extended_utf8_msg(pTHX_ const UV cp)
{
    U8 dummy[UTF8_MAXBYTES + 1];
    HV *msgs;
    SV **msg;

    uvchr_to_utf8_flags_msgs(dummy, cp, UNICODE_WARN_PERL_EXTENDED,
                             &msgs);

    msg = hv_fetchs(msgs, "text", 0);
    assert(msg);

    (void) sv_2mortal((SV *) msgs);

    return SvPVX(*msg);