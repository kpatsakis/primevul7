drill_add_drill_hole (gerbv_image_t *image, drill_state_t *state,
		gerbv_drill_stats_t *stats, gerbv_net_t *curr_net)
{
    gerbv_render_size_t *bbox;
    double r;

    /* Add one to drill stats  for the current tool */
    drill_stats_increment_drill_counter(image->drill_stats->drill_list,
	    state->current_tool);

    curr_net->next = g_new0(gerbv_net_t, 1);
    if (curr_net->next == NULL)
	GERB_FATAL_ERROR("malloc curr_net->next failed in %s()",
			__FUNCTION__);

    curr_net = curr_net->next;
    curr_net->layer = image->layers;
    curr_net->state = image->states;
    curr_net->start_x = state->curr_x;
    curr_net->start_y = state->curr_y;
    /* KLUDGE. This function isn't allowed to return anything
       but inches */
    if(state->unit == GERBV_UNIT_MM) {
	curr_net->start_x /= 25.4;
	curr_net->start_y /= 25.4;
	/* KLUDGE. All images, regardless of input format,
	   are returned in INCH format */
	curr_net->state->unit = GERBV_UNIT_INCH;
    }

    curr_net->stop_x = curr_net->start_x - state->origin_x;
    curr_net->stop_y = curr_net->start_y - state->origin_y;
    curr_net->aperture = state->current_tool;
    curr_net->aperture_state = GERBV_APERTURE_STATE_FLASH;

    /* Check if aperture is set. Ignore the below instead of
       causing SEGV... */
    if(image->aperture[state->current_tool] == NULL)
	return curr_net;

    bbox = &curr_net->boundingBox;
    r = image->aperture[state->current_tool]->parameter[0] / 2;

    /* Set boundingBox */
    bbox->left   = curr_net->start_x - r;
    bbox->right  = curr_net->start_x + r;
    bbox->bottom = curr_net->start_y - r;
    bbox->top    = curr_net->start_y + r;

    drill_update_image_info_min_max_from_bbox(image->info, bbox);

    return curr_net;
}