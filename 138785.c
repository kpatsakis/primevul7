find_modreq(preauth_system *pa_sys, struct request_pa_context *context,
            krb5_kdcpreauth_modreq **modreq_out)
{
    int i;

    *modreq_out = NULL;
    if (context == NULL)
        return KRB5KRB_ERR_GENERIC;

    for (i = 0; i < context->n_contexts; i++) {
        if (context->contexts[i].pa_system == pa_sys) {
            *modreq_out = &context->contexts[i].modreq;
            return 0;
        }
    }

    return KRB5KRB_ERR_GENERIC;
}