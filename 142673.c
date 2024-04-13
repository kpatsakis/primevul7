_row_status_state_cleanup(netsnmp_state_machine_input *input,
                 netsnmp_state_machine_step *step)
{
    rowcreate_state       *ctx;

    netsnmp_require_ptr_LRV( input, SNMPERR_ABORT );
    netsnmp_require_ptr_LRV( step, SNMPERR_ABORT );

    DEBUGMSGT(("row_create:called", "_row_status_state_cleanup, last run step was %s rc %d\n",
               step->name, step->result));

    ctx = (rowcreate_state *)input->input_context;
    if (ctx && ctx->vars)
        snmp_free_varbind( ctx->vars );

    return SNMPERR_SUCCESS;
}