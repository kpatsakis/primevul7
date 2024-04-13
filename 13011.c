drill_parse_coordinate(gerb_file_t *fd, char firstchar,
		       gerbv_image_t *image, drill_state_t *state,
		       ssize_t file_line)
{
    int read;
    gerbv_drill_stats_t *stats = image->drill_stats;

    if(state->coordinate_mode == DRILL_MODE_ABSOLUTE) {
	if (firstchar == 'X') {
	    state->curr_x = read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	    if ((read = (char)gerb_fgetc(fd)) == 'Y') {
		state->curr_y = read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	    } else {
		gerb_ungetc(fd);
	    }
	} else if (firstchar == 'Y') {
	    state->curr_y = read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	}
    } else if(state->coordinate_mode == DRILL_MODE_INCREMENTAL) {
	if (firstchar == 'X') {
	    state->curr_x += read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	    if((read = (char)gerb_fgetc(fd)) == 'Y') {
		state->curr_y += read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	    } else {
		gerb_ungetc(fd);
	    }
	} else if (firstchar == 'Y') {
	    state->curr_y += read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	}
    } else {
	gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
		_("Coordinate mode is not absolute and not incremental "
		    "at line %ld in file \"%s\""),
		file_line, fd->filename);
    }

} /* drill_parse_coordinate */