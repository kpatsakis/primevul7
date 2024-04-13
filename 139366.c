static gg_action_t gg_handle_connected(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
#if 0
	char buf[1024];
	int res;

	if (gg_send_queued_data(sess) == -1)
		return GG_ACTION_FAIL;

	res = gg_read(sess, buf, sizeof(buf));

	if (res == -1 && (errno == EAGAIN || errno == EINTR)) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() non-critical read error (errno=%d, %s)\n", errno, strerror(errno));
		return GG_ACTION_WAIT;
	}
	
	if (res == -1 || res == 0) {
		if (res == -1)
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() read error (errno=%d, %s)\n", errno, strerror(errno));
		else
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection closed\n");

		if (sess->state == GG_STATE_DISCONNECTING && res == 0) {
			e->type = GG_EVENT_DISCONNECT_ACK;
		} else if (sess->state == GG_STATE_READING_KEY) {
			e->event.failure = GG_FAILURE_INVALID;
			return GG_ACTION_FAIL;
		}

		return GG_ACTION_FAIL;
	}

	gg_debug_dump(sess, GG_DEBUG_DUMP, buf, res);

	if (gg_session_handle_data(sess, buf, res, e) == -1)
		return GG_ACTION_FAIL;

	if (sess->send_buf != NULL)
		sess->check |= GG_CHECK_WRITE;

	return GG_ACTION_WAIT;
#else
	struct gg_header *gh;

	if (gg_send_queued_data(sess) == -1)
		return GG_ACTION_FAIL;

	gh = gg_recv_packet(sess);

	if (gh == NULL) {
		if (sess->state == GG_STATE_DISCONNECTING) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection broken expectedly\n");
			e->type = GG_EVENT_DISCONNECT_ACK;
			return GG_ACTION_WAIT;
		}

		if (errno != EAGAIN) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() gg_recv_packet failed (errno=%d, %s)\n", errno, strerror(errno));
			return GG_ACTION_FAIL;
		}
	} else {
		if (gg_session_handle_packet(sess, gh->type, (const char *) gh + sizeof(struct gg_header), gh->length, e) == -1) {
			free(gh);
			return GG_ACTION_FAIL;
		}

		free(gh);
	}

	sess->check = GG_CHECK_READ;

	if (sess->send_buf != NULL)
		sess->check |= GG_CHECK_WRITE;

	return GG_ACTION_WAIT;
#endif
}