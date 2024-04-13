check_padata(krb5_context context, krb5_kdcpreauth_rock rock,
             krb5_data *req_pkt, krb5_kdc_req *request,
             krb5_enc_tkt_part *enc_tkt_reply, void **padata_context,
             krb5_pa_data ***e_data, krb5_boolean *typed_e_data,
             kdc_preauth_respond_fn respond, void *arg)
{
    struct padata_state *state;

    if (request->padata == 0) {
        (*respond)(arg, 0);
        return;
    }

    if (make_padata_context(context, padata_context) != 0) {
        (*respond)(arg, KRB5KRB_ERR_GENERIC);
        return;
    }

    state = calloc(1, sizeof(*state));
    if (state == NULL) {
        (*respond)(arg, ENOMEM);
        return;
    }
    state->respond = respond;
    state->arg = arg;
    state->context = context;
    state->rock = rock;
    state->req_pkt = req_pkt;
    state->request = request;
    state->enc_tkt_reply = enc_tkt_reply;
    state->padata_context = padata_context;
    state->e_data_out = e_data;
    state->typed_e_data_out = typed_e_data;
    state->realm = rock->rstate->realm_data;

#ifdef DEBUG
    krb5_klog_syslog (LOG_DEBUG, "checking padata");
#endif

    next_padata(state);
}