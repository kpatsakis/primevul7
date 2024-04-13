drill_parse_header_is_inch(gerb_file_t *fd, drill_state_t *state,
			gerbv_image_t *image, ssize_t file_line)
{
    gerbv_drill_stats_t *stats = image->drill_stats;
    char c;

    dprintf("    %s(): entering\n", __FUNCTION__);

    if (DRILL_HEADER != state->curr_section) 
	return 0;

    switch (file_check_str(fd, "INCH")) {
    case -1:
	gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
		_("Unexpected EOF found while parsing \"%s\" string "
		    "in file \"%s\""), "INCH", fd->filename);
	return 0;

    case 0:
	return 0;
    }

    /* Look for TZ/LZ */
    if (',' != gerb_fgetc(fd)) {
	/* Unget the char in case we just advanced past a new line char */
	gerb_ungetc (fd);
    } else {
	c = gerb_fgetc(fd);
	if (c != EOF && 'Z' == gerb_fgetc(fd)) {
	    switch (c) {
	    case 'L':
		if (state->autod) {
		    image->format->omit_zeros = GERBV_OMIT_ZEROS_TRAILING;
		    state->header_number_format =
			state->number_format = FMT_00_0000;
		    state->decimals = 4;
		}
		break;

	    case 'T':
		if (state->autod) {
		    image->format->omit_zeros = GERBV_OMIT_ZEROS_LEADING;
		    state->header_number_format =
			state->number_format = FMT_00_0000;
		    state->decimals = 4;
		}
		break;

	    default:
		gerbv_stats_printf(stats->error_list,
			GERBV_MESSAGE_WARNING, -1,
			_("Found junk '%s' after "
			    "INCH command "
			    "at line %ld in file \"%s\""),
			gerbv_escape_char(c),
			file_line, fd->filename);
		break;
	    }
	} else {
	    gerbv_stats_printf(stats->error_list,
		    GERBV_MESSAGE_WARNING, -1,
		    _("Found junk '%s' after INCH command "
			"at line %ld in file \"%s\""),
		    gerbv_escape_char(c),
		    file_line, fd->filename);
	}
    }

    state->unit = GERBV_UNIT_INCH;

    return 1;
} /* drill_parse_header_is_inch() */