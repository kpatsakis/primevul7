_row_status_state_all_values_createAndWait(netsnmp_state_machine_input *input,
                                           netsnmp_state_machine_step *step)
{
    rowcreate_state       *ctx;
    netsnmp_variable_list *vars, *rs_var;
    int                    rc;

    netsnmp_require_ptr_LRV( input, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( step, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( input->input_context, SNMPERR_GENERR );

    ctx = (rowcreate_state *)input->input_context;

    DEBUGMSGT(("row_create:called", "called %s\n", step->name));

    vars = snmp_clone_varbind(ctx->vars);
    netsnmp_require_ptr_LRV(vars, SNMPERR_GENERR);

    /*
     * make sure row stats is createAndWait
     */
    rs_var = _get_vb_num(vars, ctx->row_status_index);
    if (NULL == rs_var) {
        snmp_free_varbind(vars);
        return SNMPERR_GENERR;
    }

    if (*rs_var->val.integer != RS_CREATEANDWAIT)
        *rs_var->val.integer = RS_CREATEANDWAIT;

    /*
     * send set
     */
    rc = netsnmp_query_set( vars, ctx->session );
    if (-2 == rc)
        rc = SNMPERR_ABORT;

    snmp_free_varbind(vars);

    return rc;
}