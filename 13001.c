drill_parse_G_code(gerb_file_t *fd, gerbv_image_t *image, ssize_t file_line)
{
    char op[3];
    drill_g_code_t g_code;
    gerbv_drill_stats_t *stats = image->drill_stats;
    
    dprintf("---> entering %s()...\n", __FUNCTION__);

    op[0] = gerb_fgetc(fd);
    op[1] = gerb_fgetc(fd);
    op[2] = '\0';

    if (op[0] == EOF
    ||  op[1] == EOF) {
	gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
		_("Unexpected EOF found while parsing G-code in file \"%s\""),
		fd->filename);
	return DRILL_G_UNKNOWN;
    }

    dprintf("  Compare G-code \"%s\" at line %ld\n", op, file_line);

    switch (g_code = atoi(op)) {
    case 0:
	/* atoi() return 0 in case of error, recheck string */
	if (0 != strncmp(op, "00", 2)) {
	    g_code = DRILL_G_UNKNOWN;
	    gerb_ungetc(fd);
	    gerb_ungetc(fd);
	    break;
	}
	stats->G00++;
	break;
    case 1:
	stats->G01++;
	break;
    case 2:
	stats->G02++;
	break;
    case 3:
	stats->G03++;
	break;
    case 5:
	stats->G05++;
	break;
    case 85:
	stats->G85++;
	break;
    case 90:
	stats->G90++;
	break;
    case 91:
	stats->G91++;
	break;
    case 93:
	stats->G93++;
	break;

    case DRILL_G_UNKNOWN:
    default:
	stats->G_unknown++;
	break;
    }

    dprintf("<----  ...leaving %s()\n", __FUNCTION__);

    return g_code;
} /* drill_parse_G_code() */