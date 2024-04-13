_row_status_state_single_value_cols(netsnmp_state_machine_input *input,
                                    netsnmp_state_machine_step *step)
{
    rowcreate_state       *ctx;
    netsnmp_variable_list *var, *tmp_next, *row_status;
    int                    rc = SNMPERR_GENERR;

    netsnmp_require_ptr_LRV( input, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( step, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( input->input_context, SNMPERR_GENERR );

    ctx = (rowcreate_state *)input->input_context;

    DEBUGMSGT(("row_create:called", "called %s\n", step->name));

    row_status = _get_vb_num(ctx->vars, ctx->row_status_index);
    netsnmp_require_ptr_LRV(row_status, SNMPERR_GENERR);

    /*
     * try one varbind at a time
     */
    for (var = ctx->vars; var; var = var->next_variable) {
        if (var == row_status)
            continue;

        tmp_next = var->next_variable;
        var->next_variable = NULL;

        /*
         * send set
         */
        rc = netsnmp_query_set( var, ctx->session );
        var->next_variable = tmp_next;
        if (-2 == rc)
            rc = SNMPERR_ABORT;
        if (rc != SNMPERR_SUCCESS)
            break;
    }

    return rc;
}