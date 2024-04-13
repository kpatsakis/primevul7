static gg_action_t gg_handle_resolve_async(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	if (sess->resolver_start(&sess->fd, &sess->resolver, sess->resolver_host) == -1) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() resolving failed (errno=%d, %s)\n", errno, strerror(errno));
		e->event.failure = GG_FAILURE_RESOLVING;
		return GG_ACTION_FAIL;
	}

	sess->state = next_state;
	sess->check = GG_CHECK_READ;
	sess->timeout = GG_DEFAULT_TIMEOUT;

	return GG_ACTION_WAIT;
}