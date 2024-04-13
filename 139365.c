struct gg_event *gg_watch_fd(struct gg_session *sess)
{
	struct gg_event *ge;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_watch_fd(%p);\n", sess);

	if (sess == NULL) {
		errno = EFAULT;
		return NULL;
	}

	ge = malloc(sizeof(struct gg_event));

	if (ge == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() not enough memory for event data\n");
		return NULL;
	}

	memset(ge, 0, sizeof(struct gg_event));

	ge->type = GG_EVENT_NONE;

	for (;;) {
		unsigned int i;
		gg_action_t res;

		res = GG_ACTION_FAIL;

		for (i = 0; i < sizeof(handlers) / sizeof(handlers[0]); i++) {
			if (handlers[i].state == (enum gg_state_t) sess->state) {
				gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() %s\n", gg_debug_state(sess->state));
				res = (*handlers[i].handler)(sess, ge, handlers[i].next_state, handlers[i].alt_state, handlers[i].alt2_state);
				break;
			}
		}

		if (!sess->async && ge->type == GG_EVENT_NONE && res == GG_ACTION_WAIT)
			res = GG_ACTION_NEXT;

		switch (res) {
			case GG_ACTION_WAIT:
				return ge;

			case GG_ACTION_NEXT:
				continue;

			case GG_ACTION_FAIL:
				sess->state = GG_STATE_IDLE;

				if (sess->fd != -1) {
					int errno2;

					errno2 = errno;
					close(sess->fd);
					errno = errno2;
					sess->fd = -1;
				}

				if (ge->event.failure != 0) {
					ge->type = GG_EVENT_CONN_FAILED;
				} else {
					free(ge);
					ge = NULL;
				}

				return ge;

			/* Celowo nie ma default */
		}
	}
}