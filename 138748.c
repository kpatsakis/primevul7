hint_list_next(struct hint_state *state)
{
    preauth_system *ap = state->ap;
    kdc_realm_t *kdc_active_realm = state->realm;

    if (ap->type == -1) {
        hint_list_finish(state, 0);
        return;
    }

    if (state->hw_only && !(ap->flags & PA_HARDWARE))
        goto next;
    if (ap->flags & PA_PSEUDO)
        goto next;

    state->pa_type = ap->type;
    if (ap->get_edata) {
        ap->get_edata(kdc_context, state->request, &callbacks, state->rock,
                      ap->moddata, ap->type, finish_get_edata, state);
    } else
        finish_get_edata(state, 0, NULL);
    return;

next:
    state->ap++;
    hint_list_next(state);
}