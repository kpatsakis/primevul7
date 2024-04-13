static gg_action_t gg_handle_connecting_gg(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	int res;

	sess->soft_timeout = 0;

	/* jeśli wystąpił błąd podczas łączenia się... */
	if (gg_async_connect_failed(sess, &res)) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection failed (errno=%d, %s)\n", res, strerror(res));
		close(sess->fd);
		sess->fd = -1;
		sess->connect_index++;
		sess->state = alt_state;
		return GG_ACTION_NEXT;
	}

	free(sess->resolver_result);
	sess->resolver_result = NULL;

	gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connected\n");

	if (sess->ssl_flag != GG_SSL_DISABLED) {
		if (gg_session_init_ssl(sess) == -1) {
			e->event.failure = GG_FAILURE_TLS;
			return GG_ACTION_FAIL;
		}

		sess->state = alt2_state;
		sess->check = GG_CHECK_WRITE;
		sess->timeout = GG_DEFAULT_TIMEOUT;

		return GG_ACTION_NEXT;
	} else {
		sess->state = next_state;
		sess->check = GG_CHECK_READ;
		sess->timeout = GG_DEFAULT_TIMEOUT;

		return GG_ACTION_WAIT;
	}
}