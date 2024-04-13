hint_list_finish(struct hint_state *state, krb5_error_code code)
{
    kdc_hint_respond_fn oldrespond = state->respond;
    void *oldarg = state->arg;
    kdc_realm_t *kdc_active_realm = state->realm;

    /* Add a freshness token if a preauth module requested it and the client
     * request indicates support for it. */
    if (!code)
        code = add_freshness_token(kdc_context, state->rock, &state->pa_data);

    if (!code) {
        if (state->pa_data == NULL) {
            krb5_klog_syslog(LOG_INFO,
                             _("%spreauth required but hint list is empty"),
                             state->hw_only ? "hw" : "");
        }

        *state->e_data_out = state->pa_data;
        state->pa_data = NULL;
    }

    krb5_free_pa_data(kdc_context, state->pa_data);
    free(state);
    (*oldrespond)(oldarg);
}