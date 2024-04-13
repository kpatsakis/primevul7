next_padata(struct padata_state *state)
{
    assert(state);
    if (!state->padata)
        state->padata = state->request->padata;
    else
        state->padata++;

    if (!*state->padata) {
        finish_check_padata(state, state->saved_code);
        return;
    }

#ifdef DEBUG
    krb5_klog_syslog (LOG_DEBUG, ".. pa_type 0x%x", (*state->padata)->pa_type);
#endif
    if (find_pa_system((*state->padata)->pa_type, &state->pa_sys))
        goto next;
    if (find_modreq(state->pa_sys, *state->padata_context, &state->modreq_ptr))
        goto next;
#ifdef DEBUG
    krb5_klog_syslog (LOG_DEBUG, ".. pa_type %s", state->pa_sys->name);
#endif
    if (state->pa_sys->verify_padata == 0)
        goto next;

    state->pa_found++;
    state->pa_sys->verify_padata(state->context, state->req_pkt,
                                 state->request, state->enc_tkt_reply,
                                 *state->padata, &callbacks, state->rock,
                                 state->pa_sys->moddata, finish_verify_padata,
                                 state);
    return;

next:
    next_padata(state);
}