static gg_action_t gg_handle_connect(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	struct in_addr addr;
	int port;

	if (sess->resolver_index >= sess->resolver_count) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() out of addresses to connect to\n");
		e->event.failure = GG_FAILURE_CONNECTING;
		return GG_ACTION_FAIL;
	}

	addr = sess->resolver_result[sess->resolver_index];

	if (sess->state == GG_STATE_CONNECT_HUB) {
		port = GG_APPMSG_PORT;
	} else {
		sess->proxy_addr = addr.s_addr;
		port = sess->proxy_port;
	}

	gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connecting to %s:%d\n", inet_ntoa(addr), port);

	sess->fd = gg_connect(&addr, port, sess->async);

	if (sess->fd == -1) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection failed (errno=%d, %s)\n", errno, strerror(errno));
		sess->resolver_index++;
		return GG_ACTION_NEXT;
	}

	sess->state = next_state;
	sess->check = GG_CHECK_WRITE;
	sess->timeout = GG_DEFAULT_TIMEOUT;
	sess->soft_timeout = 1;

	return GG_ACTION_WAIT;
}