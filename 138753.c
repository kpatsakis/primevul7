requires_info2(const krb5_kdc_req *request)
{
    int i;

    for (i = 0; i < request->nktypes; i++) {
        if (enctype_requires_etype_info_2(request->ktype[i]))
            return TRUE;
    }
    return FALSE;
}