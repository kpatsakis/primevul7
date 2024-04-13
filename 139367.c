static gg_action_t gg_handle_connect_gg(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	struct in_addr addr;
	uint16_t port;

	gg_debug_session(sess, GG_DEBUG_MISC, "resolver_index=%d, connect_index=%d, connect_port={%d,%d}\n", sess->resolver_index, sess->connect_index, sess->connect_port[0], sess->connect_port[1]);

	if ((unsigned int) sess->connect_index >= sizeof(sess->connect_port) / sizeof(sess->connect_port[0]) || sess->connect_port[sess->connect_index] == 0) {
		sess->connect_index = 0;
		sess->resolver_index++;
		if (sess->resolver_index >= sess->resolver_count) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() out of addresses to connect to\n");
			e->event.failure = GG_FAILURE_CONNECTING;
			return GG_ACTION_FAIL;
		}
	}

	addr = sess->resolver_result[sess->resolver_index];
	port = sess->connect_port[sess->connect_index];

	gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connecting to %s:%d\n", inet_ntoa(addr), port);

	sess->fd = gg_connect(&addr, port, sess->async);

	if (sess->fd == -1) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection failed (errno=%d, %s)\n", errno, strerror(errno));
		sess->connect_index++;
		return GG_ACTION_NEXT;
	}

	sess->state = next_state;
	sess->check = GG_CHECK_WRITE;
	sess->timeout = GG_DEFAULT_TIMEOUT;
	sess->soft_timeout = 1;

	return GG_ACTION_WAIT;
}