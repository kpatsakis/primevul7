static gg_action_t gg_handle_connecting(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	int res;

	sess->soft_timeout = 0;

	if (gg_async_connect_failed(sess, &res)) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection failed (errno=%d, %s)\n", res, strerror(res));
		close(sess->fd);
		sess->fd = -1;
		sess->resolver_index++;
		sess->state = alt_state;
	} else {
		/* Z proxy zwykle łączymy się dwa razy, więc nie zwalniamy
		 * adresów IP po pierwszym połączeniu. */
		if (sess->state != GG_STATE_CONNECTING_PROXY_HUB) {
			free(sess->resolver_result);
			sess->resolver_result = NULL;
		}

		sess->state = next_state;
	}

	return GG_ACTION_NEXT;
}