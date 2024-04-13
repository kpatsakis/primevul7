netsnmp_row_create(netsnmp_session *sess, netsnmp_variable_list *vars,
                   int row_status_index)
{
    netsnmp_state_machine_step rc_cleanup =
        { "row_create_cleanup", 0, _row_status_state_cleanup,
          0, NULL, NULL, 0, NULL };
    netsnmp_state_machine_step rc_activate =
        { "row_create_activate", 0, _row_status_state_activate,
          0, NULL, NULL, 0, NULL };
    netsnmp_state_machine_step rc_sv_cols =
        { "row_create_single_value_cols", 0,
          _row_status_state_single_value_cols, 0, &rc_activate,NULL, 0, NULL };
    netsnmp_state_machine_step rc_mv_cols =
        { "row_create_multiple_values_cols", 0,
          _row_status_state_multiple_values_cols, 0, &rc_activate, &rc_sv_cols,
          0, NULL };
    netsnmp_state_machine_step rc_sv_caw =
        { "row_create_single_value_createAndWait", 0,
          _row_status_state_single_value_createAndWait, 0, &rc_mv_cols, NULL,
          0, NULL };
    netsnmp_state_machine_step rc_av_caw =
        { "row_create_all_values_createAndWait", 0,
          _row_status_state_all_values_createAndWait, 0, &rc_activate,
          &rc_sv_caw, 0, NULL };
    netsnmp_state_machine_step rc_av_cag =
        { "row_create_all_values_createAndGo", 0,
          _row_status_state_all_values_createAndGo, 0, NULL, &rc_av_caw, 0,
          NULL };

    netsnmp_state_machine_input sm_input = { "row_create_machine", 0,
                                             &rc_av_cag, &rc_cleanup };
    rowcreate_state state;

    netsnmp_require_ptr_LRV( sess, SNMPERR_GENERR);
    netsnmp_require_ptr_LRV( vars, SNMPERR_GENERR);

    state.session = sess;
    state.vars = vars;

    state.row_status_index = row_status_index;
    sm_input.input_context = &state;

    netsnmp_state_machine_run( &sm_input);

    return SNMPERR_SUCCESS;
}