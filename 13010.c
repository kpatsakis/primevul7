drill_parse_M_code(gerb_file_t *fd, drill_state_t *state,
	gerbv_image_t *image, ssize_t file_line)
{
    gerbv_drill_stats_t *stats = image->drill_stats;
    drill_m_code_t m_code;
    char op[3];

    dprintf("---> entering %s() ...\n", __FUNCTION__);

    op[0] = gerb_fgetc(fd);
    op[1] = gerb_fgetc(fd);
    op[2] = '\0';

    if (op[0] == EOF
    ||  op[1] == EOF) {
	gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
		_("Unexpected EOF found while parsing M-code in file \"%s\""),
		fd->filename);

	return DRILL_M_UNKNOWN;
    }

    dprintf("  Compare M-code \"%s\" at line %ld\n", op, file_line);

    switch (m_code = atoi(op)) {
    case 0:
	/* atoi() return 0 in case of error, recheck string */
	if (0 != strncmp(op, "00", 2)) {
	    m_code = DRILL_M_UNKNOWN;
	    gerb_ungetc(fd);
	    gerb_ungetc(fd);
	    break;
	}
	stats->M00++;
	break;
    case 1:
	stats->M01++;
	break;
    case 18:
	stats->M18++;
	break;
    case 25:
	stats->M25++;
	break;
    case 30:
	stats->M30++;
	break;
    case 45:
	stats->M45++;
	break;
    case 47:
	stats->M47++;
	break;
    case 48:
	stats->M48++;
	break;
    case 71:
	stats->M71++;
	eat_line(fd);
	break;
    case 72:
	stats->M72++;
	eat_line(fd);
	break;
    case 95:
	stats->M95++;
	break;
    case 97:
	stats->M97++;
	break;
    case 98:
	stats->M98++;
	break;

    default:
    case DRILL_M_UNKNOWN:
	break;
    }


    dprintf("<----  ...leaving %s()\n", __FUNCTION__);

    return m_code;
} /* drill_parse_M_code() */