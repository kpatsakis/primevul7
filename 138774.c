free_padata_context(krb5_context kcontext, void *padata_context)
{
    struct request_pa_context *context = padata_context;
    preauth_system *sys;
    int i;

    if (context == NULL)
        return;
    for (i = 0; i < context->n_contexts; i++) {
        sys = context->contexts[i].pa_system;
        if (!sys->free_modreq || !context->contexts[i].modreq)
            continue;
        sys->free_modreq(kcontext, sys->moddata, context->contexts[i].modreq);
        context->contexts[i].modreq = NULL;
    }

    free(context->contexts);
    free(context);
}