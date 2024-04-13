parse_drillfile(gerb_file_t *fd, gerbv_HID_Attribute *attr_list, int n_attr, int reload)
{
    drill_state_t *state = NULL;
    gerbv_image_t *image = NULL;
    gerbv_net_t *curr_net = NULL;
    gerbv_HID_Attribute *hid_attrs;
    int read;
    gerbv_drill_stats_t *stats;
    gchar *tmps;
    ssize_t file_line = 1;

    /* 
     * many locales redefine "." as "," and so on, so sscanf and strtod 
     * has problems when reading files using %f format.
     * Fixes bug #1963618 reported by Lorenzo Marcantonio.
     */
    setlocale(LC_NUMERIC, "C" );

    /* Create new image for this layer */
    dprintf("In parse_drillfile, about to create image for this layer\n");

    image = gerbv_create_image(image, "Excellon Drill File");
    if (image == NULL)
	GERB_FATAL_ERROR("malloc image failed in %s()", __FUNCTION__);

    if (reload && attr_list != NULL) {
      /* FIXME there should probably just be a function to copy an
	 attribute list including using strdup as needed */

	image->info->n_attr = n_attr;
	image->info->attr_list = gerbv_attribute_dup(attr_list, n_attr);

    } else {
	/* Copy in the default attribute list for drill files.  We make a
	 * copy here because we will allow per-layer editing of the
	 * attributes.
	 */
	image->info->n_attr = sizeof (drill_attribute_list) / sizeof (drill_attribute_list[0]);
	image->info->attr_list = gerbv_attribute_dup (drill_attribute_list, image->info->n_attr);

	/* now merge any project attributes */
	drill_attribute_merge (image->info->attr_list, image->info->n_attr,
			 attr_list, n_attr);
    }
    
    curr_net = image->netlist;
    curr_net->layer = image->layers;
    curr_net->state = image->states;
    image->layertype = GERBV_LAYERTYPE_DRILL;
    stats = gerbv_drill_stats_new();
    if (stats == NULL)
	GERB_FATAL_ERROR("malloc stats failed in %s()", __FUNCTION__);
    image->drill_stats = stats;

    /* Create local state variable to track photoplotter state */
    state = new_state(state);
    if (state == NULL)
	GERB_FATAL_ERROR("malloc state failed in %s()", __FUNCTION__);

    image->format = g_new0(gerbv_format_t, 1);
    if (image->format == NULL)
	GERB_FATAL_ERROR("malloc format failed in %s()", __FUNCTION__);

    image->format->omit_zeros = GERBV_OMIT_ZEROS_UNSPECIFIED;

    hid_attrs = image->info->attr_list;

    if (!hid_attrs[HA_auto].default_val.int_value) {
	state->autod = 0;
	state->number_format = FMT_USER;
	state->decimals = hid_attrs[HA_digits].default_val.int_value;

	if (GERBV_UNIT_MM == hid_attrs[HA_xy_units].default_val.int_value)
	    state->unit = GERBV_UNIT_MM;

	switch (hid_attrs[HA_suppression].default_val.int_value) {
	case SUP_LEAD:
	    image->format->omit_zeros = GERBV_OMIT_ZEROS_LEADING;
	    break;
	    
	case SUP_TRAIL:
	    image->format->omit_zeros = GERBV_OMIT_ZEROS_TRAILING;
	    break;

	default:
	    image->format->omit_zeros = GERBV_OMIT_ZEROS_EXPLICIT;
	    break;
	}
    }

    dprintf("%s():  Starting parsing of drill file \"%s\"\n",
		    __FUNCTION__, fd->filename);

    while ((read = gerb_fgetc(fd)) != EOF) {

	switch ((char) read) {

	case ';' :
	    /* Comment found. Eat rest of line */
	    tmps = get_line(fd);
	    gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_NOTE, -1,
		    _("Comment \"%s\" at line %ld in file \"%s\""),
		    tmps, file_line, fd->filename);
	    dprintf("    Comment with ';' \"%s\" at line %ld\n",
		    tmps, file_line);
	    g_free(tmps);
	    break;

	case 'D' :
	    gerb_ungetc (fd);
	    tmps = get_line (fd);
	    if (strcmp (tmps, "DETECT,ON") == 0 ||
		strcmp (tmps, "DETECT,OFF") == 0) {
		gchar *tmps2;
		gchar *tmps3;
		if (strcmp (tmps, "DETECT,ON") == 0)
		    tmps3 = "ON";
		else
		    tmps3 = "OFF";

		/* broken tool detect on/off.  Silently ignored. */
		if (stats->detect) {
		    tmps2 = g_strdup_printf ("%s\n%s", stats->detect, tmps3);
		    g_free (stats->detect);
		} else {
		    tmps2 = g_strdup_printf ("%s", tmps3);
		}
		stats->detect = tmps2;
	    } else {
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
			_("Unrecognised string \"%s\" in header "
			    "at line %ld in file \"%s\""),
			tmps, file_line, fd->filename);
	    }
	    g_free (tmps);
	    break;

	case 'F' :
	    gerb_ungetc (fd);
	    tmps = get_line (fd);

	    /* Silently ignore FMAT,2.  Not sure what others are allowed */
	    if (0 == strcmp (tmps, "FMAT,2")) {
		g_free (tmps);
		break;
	    }

	    if (0 == strcmp (tmps, "FMAT,1")) {
		gerbv_stats_printf(stats->error_list,
			GERBV_MESSAGE_ERROR, -1,
			_("File in unsupported format 1 "
			    "at line %ld in file \"%s\""),
			file_line, fd->filename);

		gerbv_destroy_image(image);
		g_free (tmps);

		return NULL;
	    }
	    
	    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_ERROR, -1,
			    _("Unrecognised string \"%s\" in header "
				    "at line %ld in file \"%s\""),
			    tmps, file_line, fd->filename);

	    g_free (tmps);
	    break;

	case 'G': {
	    /* Most G codes aren't used, for now */
	    drill_g_code_t  g_code;

	    switch (g_code = drill_parse_G_code(fd, image, file_line)) {

	    case DRILL_G_DRILL :
		/* Drill mode */
		break;

	    case DRILL_G_SLOT : {
		/* Parse drilled slot end coords */
		gerbv_render_size_t *bbox = &curr_net->boundingBox;
		double r;

		if (EOF == (read = gerb_fgetc(fd))) {
		    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_ERROR, -1,
			    _("Unexpected EOF found in file \"%s\""),
			    fd->filename);
		    break;
		}

		drill_parse_coordinate(fd, read, image, state, file_line);

		/* Modify last curr_net as drilled slot */
		curr_net->stop_x = state->curr_x;
		curr_net->stop_y = state->curr_y;

		if (state->unit == GERBV_UNIT_MM) {
		    /* Convert to inches -- internal units */
		    curr_net->stop_x /= 25.4;
		    curr_net->stop_y /= 25.4;
		}

		r = image->aperture[state->current_tool]->parameter[0]/2;

		/* Update boundingBox with drilled slot stop_x,y coords */
		bbox->left   = MIN(bbox->left,   curr_net->stop_x - r);
		bbox->right  = MAX(bbox->right,  curr_net->stop_x + r);
		bbox->bottom = MIN(bbox->bottom, curr_net->stop_y - r);
		bbox->top    = MAX(bbox->top,    curr_net->stop_y + r);

		drill_update_image_info_min_max_from_bbox(image->info, bbox);

		curr_net->aperture_state = GERBV_APERTURE_STATE_ON;

		break;
	    }

	    case DRILL_G_ABSOLUTE :
		state->coordinate_mode = DRILL_MODE_ABSOLUTE;
		break;

	    case DRILL_G_INCREMENTAL :
		state->coordinate_mode = DRILL_MODE_INCREMENTAL;
		break;

	    case DRILL_G_ZEROSET :
		if (EOF == (read = gerb_fgetc(fd))) {
		    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_ERROR, -1,
			    _("Unexpected EOF found in file \"%s\""),
			    fd->filename);
		    break;
		}

		drill_parse_coordinate(fd, (char)read, image,
			state, file_line);
		state->origin_x = state->curr_x;
		state->origin_y = state->curr_y;
		break;

	    case DRILL_G_UNKNOWN:
		tmps = get_line(fd);
		gerbv_stats_printf(stats->error_list,
			GERBV_MESSAGE_ERROR, -1,
			_("Unrecognized string \"%s\" found "
			    "at line %ld in file \"%s\""),
			tmps, file_line, fd->filename);
		g_free(tmps);
		break;

	    default:
		eat_line(fd);
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
			_("Unsupported G%02d (%s) code "
			    "at line %ld in file \"%s\""),
			g_code, drill_g_code_name(g_code),
			file_line, fd->filename);
		break;
	    }

	    break;
	}

	case 'I':
	    gerb_ungetc(fd); /* To compare full string in function or
				report full string  */
	    if (drill_parse_header_is_inch(fd, state, image, file_line))
		break;

	    if (drill_parse_header_is_ici(fd, state, image, file_line))
		break;

	    tmps = get_line(fd);
	    gerbv_stats_printf(stats->error_list,
		    GERBV_MESSAGE_ERROR, -1,
		    _("Unrecognized string \"%s\" found "
			"at line %ld in file \"%s\""),
		    tmps, file_line, fd->filename);
	    g_free(tmps);

	    break;

	case 'M': {
	    int m_code;

	    switch (m_code = drill_parse_M_code(fd, state, image, file_line)) {
	    case DRILL_M_HEADER :
		state->curr_section = DRILL_HEADER;
		break;
	    case DRILL_M_HEADEREND :
		state->curr_section = DRILL_DATA;

		if (image->format->omit_zeros == GERBV_OMIT_ZEROS_UNSPECIFIED) {
		    /* Excellon says they default to specify leading
		       zeros, i.e. omit trailing zeros.	 The Excellon
		       files floating around that don't specify the
		       leading/trailing zeros in the header seem to
		       contradict to this though.

		       XXX We should probably ask the user. */

		    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_ERROR, -1,
			    _("End of Excellon header reached "
				"but no leading/trailing zero "
				"handling specified "
				"at line %ld in file \"%s\""),
			    file_line, fd->filename);
		    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_WARNING, -1,
			    _("Assuming leading zeros in file \"%s\""),
			    fd->filename);
		    image->format->omit_zeros = GERBV_OMIT_ZEROS_LEADING;
		}
		break;
	    case DRILL_M_METRIC :
		if (state->unit == GERBV_UNIT_UNSPECIFIED
		&&  state->curr_section != DRILL_HEADER) {
		    double size;

		    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_WARNING, -1,
			    _("M71 code found but no METRIC "
				"specification in header "
				"at line %ld in file \"%s\""),
			    file_line, fd->filename);
		    gerbv_stats_printf(stats->error_list,
			    GERBV_MESSAGE_WARNING, -1,
			    _("Assuming all tool sizes are MM in file \"%s\""),
			    fd->filename);

		    stats = image->drill_stats;
		    for (int tool_num = TOOL_MIN; tool_num < TOOL_MAX;
			    tool_num++) {
			if (image->aperture && image->aperture[tool_num]) {
			    /* First update stats.   Do this before changing drill dias.
			     * Maybe also put error into stats? */
			    size = image->aperture[tool_num]->parameter[0];
			    drill_stats_modify_drill_list(stats->drill_list, 
							  tool_num, 
							  size, 
							  "MM");
			    /* Now go back and update all tool dias, since
			     * tools are displayed in inch units
			     */
			    image->aperture[tool_num]->parameter[0] /= 25.4;
			}
		    }
		}
		if (state->autod) {
		    state->number_format = state->backup_number_format;
		    state->unit = GERBV_UNIT_MM;
		}
		break;
	    case DRILL_M_IMPERIAL :
		if (state->autod) {
		    if (state->number_format != FMT_00_0000)
			/* save metric format definition for later */
			state->backup_number_format = state->number_format;
		    state->number_format = FMT_00_0000;
		    state->decimals = 4;
		    state->unit = GERBV_UNIT_INCH;
		}

		break;
	    case DRILL_M_CANNEDTEXTX :
	    case DRILL_M_CANNEDTEXTY :
		tmps = get_line(fd);
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_NOTE, -1,
			_("Canned text \"%s\" "
			    "at line %ld in drill file \"%s\""),
			tmps, file_line, fd->filename);
		g_free(tmps);
		break;
	    case DRILL_M_MESSAGELONG :
	    case DRILL_M_MESSAGE :
		tmps = get_line(fd);
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_NOTE, -1,
			_("Message \"%s\" embedded "
			    "at line %ld in drill file \"%s\""),
			tmps, file_line, fd->filename);
		g_free(tmps);
		break;
	    case DRILL_M_PATTERNEND :
	    case DRILL_M_TOOLTIPCHECK :
		break;

	    case DRILL_M_END :
		/* M00 has optional arguments */
		eat_line(fd);

	    case DRILL_M_ENDREWIND :
		goto drill_parse_end;
		break;

	    case DRILL_M_UNKNOWN:
		gerb_ungetc(fd); /* To compare full string in function or
				    report full string  */
		if (drill_parse_header_is_metric(fd, state, image, file_line))
		    break;

		stats->M_unknown++;
		tmps = get_line(fd);
		gerbv_stats_printf(stats->error_list,
			GERBV_MESSAGE_ERROR, -1,
			_("Unrecognized string \"%s\" found "
			    "at line %ld in file \"%s\""),
			tmps, file_line, fd->filename);
		g_free(tmps);

		break;

	    default:
		stats->M_unknown++;
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
			_("Unsupported M%02d (%s) code found "
			    "at line %ld in file \"%s\""),
			m_code, _(drill_m_code_name(m_code)),
			file_line, fd->filename);
		break;
	    } /* switch(m_code) */

	    break;
	} /* case 'M' */

	case 'R':
	    if (state->curr_section == DRILL_HEADER) {
		stats->unknown++;
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
			_("Not allowed 'R' code in the header "
			    "at line %ld in file \"%s\""),
			file_line, fd->filename);
	    } else {
	      double start_x, start_y;
	      double step_x, step_y;
	      int c;
	      int rcnt;
	      /*
	       * This is the "Repeat hole" command.  Format is:
	       * R##[X##][Y##]
	       * This repeats the previous hole stepping in the X and
	       * Y increments give.  Example:
	       * R04X0.1 -- repeats the drill hole 4 times, stepping
	       * 0.1" in the X direction.  Note that the X and Y step
	       * sizes default to zero if not given and that they use
	       * the same format and units as the normal X,Y
	       * coordinates.
	       */
	      stats->R++;

	      start_x = state->curr_x;
	      start_y = state->curr_y;

	      /* figure out how many repeats there are */
	      c = gerb_fgetc (fd);
	      rcnt = 0;
	      while ( '0' <= c && c <= '9') {
		rcnt = 10*rcnt + (c - '0');
		c = gerb_fgetc (fd);
	      }
	      dprintf ("working on R code (repeat) with a number of reps equal to %d\n", rcnt);

	      step_x = 0.0;
	      if (c == 'X') {
		step_x = read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
		c = gerb_fgetc (fd);
	      }

	      step_y = 0.0;
	      if( c == 'Y') {
		  step_y = read_double(fd, state->number_format, image->format->omit_zeros, state->decimals);
	      } else {
		gerb_ungetc (fd);
	      }
	      
	      dprintf ("Getting ready to repeat the drill %d times with delta_x = %g, delta_y = %g\n", rcnt, step_x, step_y);

	      /* spit out the drills */
	      for (c = 1 ; c <= rcnt ; c++) {
		state->curr_x = start_x + c*step_x;
		state->curr_y = start_y + c*step_y;
		dprintf ("    Repeat #%d - new location is (%g, %g)\n", c, state->curr_x, state->curr_y);
		curr_net = drill_add_drill_hole (image, state, stats, curr_net);
	      }
	      
	    }

	case 'S':
	    gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_NOTE, -1,
		    _("Ignoring setting spindle speed "
			"at line %ld in drill file \"%s\""),
		    file_line, fd->filename);
	    eat_line(fd);
	    break;
	case 'T':
	    drill_parse_T_code(fd, state, image, file_line);
	    break;
	case 'V' :
	    gerb_ungetc (fd);
	    tmps = get_line (fd);
	    /* Silently ignore VER,1.  Not sure what others are allowed */
	    if (0 != strcmp (tmps, "VER,1")) {
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_NOTE, -1,
			_("Undefined string \"%s\" in header "
			    "at line %ld in file \"%s\""),
			tmps, file_line, fd->filename);
	    }
	    g_free (tmps);
	    break;

	case 'X':
	case 'Y':
	    /* Hole coordinate found. Do some parsing */
	    drill_parse_coordinate(fd, read, image, state, file_line);
	    
	    /* add the new drill hole */
	    curr_net = drill_add_drill_hole (image, state, stats, curr_net);
	    break;

	case '%':
	    state->curr_section = DRILL_DATA;
	    break;

	case '\n' :
	    file_line++;

	    /* Get <CR> char, if any, from <LF><CR> pair */
	    read = gerb_fgetc(fd);
	    if (read != '\r' && read != EOF)
		    gerb_ungetc(fd);
	    break;

	case '\r' :
	    file_line++;

	    /* Get <LF> char, if any, from <CR><LF> pair */
	    read = gerb_fgetc(fd);
	    if (read != '\n' && read != EOF)
		    gerb_ungetc(fd);
	    break;

	case ' ' :	/* White space */
	case '\t' :
	    break;

	default:
	    stats->unknown++;

	    if (DRILL_HEADER == state->curr_section) {
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
			_("Undefined code '%s' (0x%x) found in header "
			    "at line %ld in file \"%s\""),
			gerbv_escape_char(read), read,
			file_line, fd->filename);
		gerb_ungetc(fd);

		/* Unrecognised crap in the header is thrown away */
		tmps = get_line(fd);
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_WARNING, -1,
			_("Unrecognised string \"%s\" in header "
			    "at line %ld in file \"%s\""),
			tmps, file_line, fd->filename);
	  	g_free (tmps);
	    } else {
		gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
			_("Ignoring undefined character '%s' (0x%x) "
			    "found inside data at line %ld in file \"%s\""),
			gerbv_escape_char(read), read, file_line, fd->filename);
	    }
	}
    }

    gerbv_stats_printf(stats->error_list, GERBV_MESSAGE_ERROR, -1,
	    _("No EOF found in drill file \"%s\""), fd->filename);

drill_parse_end:
    dprintf ("%s():  Populating file attributes\n", __FUNCTION__);

    hid_attrs = image->info->attr_list;

    switch (state->unit) {
    case GERBV_UNIT_MM:
	hid_attrs[HA_xy_units].default_val.int_value = GERBV_UNIT_MM;
	break;

    default:
	hid_attrs[HA_xy_units].default_val.int_value = GERBV_UNIT_INCH;
	break;
    }

    switch (state->number_format) {
    case FMT_000_00:
    case FMT_0000_00:
	hid_attrs[HA_digits].default_val.int_value = 2;
	break;

    case FMT_000_000:
	hid_attrs[HA_digits].default_val.int_value = 3;
	break;
	
    case FMT_00_0000:
	hid_attrs[HA_digits].default_val.int_value = 4;
	break;
	
    case FMT_USER:
	dprintf ("%s():  Keeping user specified number of decimal places (%d)\n",
		 __FUNCTION__,
		 hid_attrs[HA_digits].default_val.int_value);
	break;
	
    default:
	break;
    }

    switch (image->format->omit_zeros) {
    case GERBV_OMIT_ZEROS_LEADING:
	hid_attrs[HA_suppression].default_val.int_value = SUP_LEAD;
	break;
	    
    case GERBV_OMIT_ZEROS_TRAILING:
	hid_attrs[HA_suppression].default_val.int_value = SUP_TRAIL;
	break;

    default:
	hid_attrs[HA_suppression].default_val.int_value = SUP_NONE;
	break;
    }

    g_free(state);

    return image;
} /* parse_drillfile */