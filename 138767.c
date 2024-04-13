have_client_keys(krb5_context context, krb5_kdcpreauth_rock rock)
{
    krb5_kdc_req *request = rock->request;
    krb5_key_data *kd;
    int i;

    for (i = 0; i < request->nktypes; i++) {
        if (krb5_dbe_find_enctype(context, rock->client, request->ktype[i],
                                  -1, 0, &kd) == 0)
            return TRUE;
    }
    return FALSE;
}