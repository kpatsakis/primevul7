finish_verify_padata(void *arg, krb5_error_code code,
                     krb5_kdcpreauth_modreq modreq, krb5_pa_data **e_data,
                     krb5_authdata **authz_data)
{
    struct padata_state *state = arg;
    const char *emsg;
    krb5_boolean typed_e_data_flag;

    assert(state);
    *state->modreq_ptr = modreq;

    if (code) {
        emsg = krb5_get_error_message(state->context, code);
        krb5_klog_syslog(LOG_INFO, "preauth (%s) verify failure: %s",
                         state->pa_sys->name, emsg);
        krb5_free_error_message(state->context, emsg);

        /* Ignore authorization data returned from modules that fail */
        if (authz_data != NULL) {
            krb5_free_authdata(state->context, authz_data);
            authz_data = NULL;
        }

        typed_e_data_flag = ((state->pa_sys->flags & PA_TYPED_E_DATA) != 0);

        /*
         * We'll return edata from either the first PA_REQUIRED module
         * that fails, or the first non-PA_REQUIRED module that fails.
         * Hang on to edata from the first non-PA_REQUIRED module.
         * If we've already got one saved, simply discard this one.
         */
        if (state->pa_sys->flags & PA_REQUIRED) {
            /* free up any previous edata we might have been saving */
            if (state->pa_e_data != NULL)
                krb5_free_pa_data(state->context, state->pa_e_data);
            state->pa_e_data = e_data;
            state->typed_e_data_flag = typed_e_data_flag;

            /* Make sure we use the current retval */
            state->pa_ok = 0;
            finish_check_padata(state, code);
            return;
        } else if (state->pa_e_data == NULL) {
            /* save the first error code and e-data */
            state->pa_e_data = e_data;
            state->typed_e_data_flag = typed_e_data_flag;
            state->saved_code = code;
        } else if (e_data != NULL) {
            /* discard this extra e-data from non-PA_REQUIRED module */
            krb5_free_pa_data(state->context, e_data);
        }
    } else {
#ifdef DEBUG
        krb5_klog_syslog (LOG_DEBUG, ".. .. ok");
#endif

        /* Ignore any edata returned on success */
        if (e_data != NULL)
            krb5_free_pa_data(state->context, e_data);

        /* Add any authorization data to the ticket */
        if (authz_data != NULL) {
            add_authorization_data(state->enc_tkt_reply, authz_data);
            free(authz_data);
        }

        state->pa_ok = 1;
        if (state->pa_sys->flags & PA_SUFFICIENT) {
            finish_check_padata(state, state->saved_code);
            return;
        }
    }

    next_padata(state);
}