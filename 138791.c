finish_get_edata(void *arg, krb5_error_code code, krb5_pa_data *pa)
{
    krb5_error_code ret;
    struct hint_state *state = arg;

    if (code == 0) {
        if (pa == NULL) {
            ret = alloc_pa_data(state->pa_type, 0, &pa);
            if (ret)
                goto error;
        }
        /* add_pa_data_element() claims pa on success or failure. */
        ret = add_pa_data_element(&state->pa_data, pa);
        if (ret)
            goto error;
    }

    state->ap++;
    hint_list_next(state);
    return;

error:
    hint_list_finish(state, ret);
}