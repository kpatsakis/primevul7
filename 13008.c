drill_parse_header_is_ici(gerb_file_t *fd, drill_state_t *state,
			gerbv_image_t *image, ssize_t file_line)
{
    gerbv_drill_stats_t *stats = image->drill_stats;

    switch (file_check_str(fd, "ICI,ON")) {
    case -1:
	gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
		_("Unexpected EOF found while parsing \"%s\" string "
		    "in file \"%s\""), "ICI,ON", fd->filename);
	return 0;

    case 1:
	state->coordinate_mode = DRILL_MODE_INCREMENTAL;
	return 1;
    }

    switch (file_check_str(fd, "ICI,OFF")) {
    case -1:
	gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
		_("Unexpected EOF found while parsing \"%s\" string "
		    "in file \"%s\""), "ICI,OFF", fd->filename);
	return 0;

    case 1:
	state->coordinate_mode = DRILL_MODE_ABSOLUTE;
	return 1;
    }

    return 0;
} /* drill_parse_header_is_ici() */