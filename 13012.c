new_state(drill_state_t *state)
{
    state = g_new0(drill_state_t, 1);
    if (state != NULL) {
	/* Init structure */
	state->curr_section = DRILL_NONE;
	state->coordinate_mode = DRILL_MODE_ABSOLUTE;
	state->origin_x = 0.0;
	state->origin_y = 0.0;
	state->unit = GERBV_UNIT_UNSPECIFIED;
	state->backup_number_format = FMT_000_000; /* only used for METRIC */
	state->header_number_format = state->number_format = FMT_00_0000; /* i. e. INCH */
	state->autod = 1;
	state->decimals = 4;
    }

    return state;
} /* new_state */