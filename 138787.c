get_preauth_hint_list(krb5_kdc_req *request, krb5_kdcpreauth_rock rock,
                      krb5_pa_data ***e_data_out, kdc_hint_respond_fn respond,
                      void *arg)
{
    kdc_realm_t *kdc_active_realm = rock->rstate->realm_data;
    struct hint_state *state;
    krb5_pa_data *pa;

    *e_data_out = NULL;

    /* Allocate our state. */
    state = calloc(1, sizeof(*state));
    if (state == NULL)
        goto error;
    state->hw_only = isflagset(rock->client->attributes,
                               KRB5_KDB_REQUIRES_HW_AUTH);
    state->respond = respond;
    state->arg = arg;
    state->request = request;
    state->rock = rock;
    state->realm = rock->rstate->realm_data;
    state->e_data_out = e_data_out;
    state->pa_data = NULL;
    state->ap = preauth_systems;

    /* Add an empty PA-FX-FAST element to advertise FAST support. */
    if (alloc_pa_data(KRB5_PADATA_FX_FAST, 0, &pa) != 0)
        goto error;
    /* add_pa_data_element() claims pa on success or failure. */
    if (add_pa_data_element(&state->pa_data, pa) != 0)
        goto error;

    if (add_etype_info(kdc_context, rock, &state->pa_data) != 0)
        goto error;

    hint_list_next(state);
    return;

error:
    if (state != NULL)
        krb5_free_pa_data(kdc_context, state->pa_data);
    free(state);
    (*respond)(arg);
}