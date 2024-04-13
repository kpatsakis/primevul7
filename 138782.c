maybe_add_etype_info2(struct padata_state *state, krb5_error_code code)
{
    krb5_error_code ret;
    krb5_context context = state->context;
    krb5_kdcpreauth_rock rock = state->rock;
    krb5_pa_data *pa;

    /* Only add key information when requesting another preauth round trip. */
    if (code != KRB5KDC_ERR_MORE_PREAUTH_DATA_REQUIRED)
        return 0;

    /* Don't try to add key information when there is no key. */
    if (rock->client_key == NULL)
        return 0;

    /* If the client sent a cookie, it has already seen a KDC response with key
     * information. */
    if (krb5int_find_pa_data(context, state->request->padata,
                             KRB5_PADATA_FX_COOKIE) != NULL)
        return 0;

    ret = make_etype_info(context, KRB5_PADATA_ETYPE_INFO2,
                          rock->client->princ, rock->client_key,
                          rock->client_keyblock->enctype, &pa);
    if (ret)
        return ret;

    /* add_pa_data_element() claims pa on success or failure. */
    return add_pa_data_element(&state->pa_e_data, pa);
}