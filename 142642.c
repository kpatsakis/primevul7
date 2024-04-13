_row_status_state_multiple_values_cols(netsnmp_state_machine_input *input,
                                       netsnmp_state_machine_step *step)
{
    rowcreate_state       *ctx;
    netsnmp_variable_list *vars, *var, *last, *row_status;
    int                    rc;

    netsnmp_require_ptr_LRV( input, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( step, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( input->input_context, SNMPERR_GENERR );

    ctx = (rowcreate_state *)input->input_context;

    DEBUGMSGT(("row_create:called", "called %s\n", step->name));

    vars = snmp_clone_varbind(ctx->vars);
    netsnmp_require_ptr_LRV(vars, SNMPERR_GENERR);

    row_status = _get_vb_num(vars, ctx->row_status_index);
    if (NULL == row_status) {
        snmp_free_varbind(vars);
        return SNMPERR_GENERR;
    }

    /*
     * remove row status varbind
     */
    if (row_status == vars) {
        vars = row_status->next_variable;
        row_status->next_variable = NULL;
    }
    else {
        for (last=vars, var=last->next_variable;
             var;
             last=var, var = var->next_variable) {
            if (var == row_status) {
                last->next_variable = var->next_variable;
                break;
            }
        }
    }
    snmp_free_var(row_status);

    /*
     * send set
     */
    rc = netsnmp_query_set( vars, ctx->session );
    if (-2 == rc)
        rc = SNMPERR_ABORT;

    snmp_free_varbind(vars);

    return rc;
}