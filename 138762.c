finish_check_padata(struct padata_state *state, krb5_error_code code)
{
    kdc_preauth_respond_fn respond;
    void *arg;

    if (state->pa_ok || !state->pa_found) {
        /* Return successfully.  If we didn't match a preauth system, we may
         * return PREAUTH_REQUIRED later, but we didn't fail to verify. */
        code = 0;
        goto cleanup;
    }

    /* Add key information to the saved error pa-data if required. */
    if (maybe_add_etype_info2(state, code) != 0) {
        code = KRB5KDC_ERR_PREAUTH_FAILED;
        goto cleanup;
    }

    /* Return any saved error pa-data, stealing the pointer from state. */
    *state->e_data_out = state->pa_e_data;
    *state->typed_e_data_out = state->typed_e_data_flag;
    state->pa_e_data = NULL;

cleanup:
    /* Discard saved error pa-data if we aren't returning it, free state, and
     * respond to the AS-REQ processing code. */
    respond = state->respond;
    arg = state->arg;
    krb5_free_pa_data(state->context, state->pa_e_data);
    free(state);
    (*respond)(arg, filter_preauth_error(code));
}