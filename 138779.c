find_pa_system(int type, preauth_system **preauth)
{
    preauth_system *ap;

    if (preauth_systems == NULL)
        return KRB5_PREAUTH_BAD_TYPE;
    ap = preauth_systems;
    while ((ap->type != -1) && (ap->type != type))
        ap++;
    if (ap->type == -1)
        return(KRB5_PREAUTH_BAD_TYPE);
    *preauth = ap;
    return 0;
}