_row_status_state_single_value_createAndWait(netsnmp_state_machine_input *input,
                                             netsnmp_state_machine_step *step)
{
    rowcreate_state       *ctx;
    netsnmp_variable_list *var = NULL, *rs_var;
    int32_t                rc, val = RS_CREATEANDWAIT;

    netsnmp_require_ptr_LRV( input, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( step, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( input->input_context, SNMPERR_GENERR );

    ctx = (rowcreate_state *)input->input_context;

    DEBUGMSGT(("row_create:called", "called %s\n", step->name));

    rs_var = _get_vb_num(ctx->vars, ctx->row_status_index);
    netsnmp_require_ptr_LRV(rs_var, SNMPERR_GENERR);

    var = snmp_varlist_add_variable(&var, rs_var->name, rs_var->name_length,
                                    rs_var->type, &val, sizeof(val));
    netsnmp_require_ptr_LRV(var, SNMPERR_GENERR);

    /*
     * send set
     */
    rc = netsnmp_query_set( var, ctx->session );
    if (-2 == rc)
        rc = SNMPERR_ABORT;

    snmp_free_varbind(var);

    return rc;
}