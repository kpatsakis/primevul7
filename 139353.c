static gg_action_t gg_handle_reading_proxy_gg(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	char buf[256];
	int res;
	int reply;
	char *body;

	res = recv(sess->fd, buf, sizeof(buf), 0);

	gg_debug_session(sess, GG_DEBUG_MISC, "recv() = %d\n", res);

	if (res == -1 && (errno == EAGAIN || errno == EINTR)) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() non-critical recv error (errno=%d, %s)\n", errno, strerror(errno));
		return GG_ACTION_WAIT;
	}

	if (res == -1) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() recv error (errno=%d, %s)\n", errno, strerror(errno));
		e->event.failure = GG_FAILURE_CONNECTING;
		return GG_ACTION_FAIL;
	}

	if (res != 0) {
		char *tmp;

		tmp = realloc(sess->recv_buf, sess->recv_done + res + 1);

		if (tmp == NULL) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() not enough memory for http reply\n");
			return GG_ACTION_FAIL;
		}

		sess->recv_buf = tmp;
		memcpy(sess->recv_buf + sess->recv_done, buf, res);
		sess->recv_done += res;
		sess->recv_buf[sess->recv_done] = 0;
	}

	if (res == 0 && sess->recv_buf == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection closed\n");
		e->event.failure = GG_FAILURE_CONNECTING;
		return GG_ACTION_FAIL;
	}

	/* szukamy początku treści */
	body = strstr(sess->recv_buf, "\r\n\r\n");
	
	if (body == NULL) {
		body = strstr(sess->recv_buf, "\n\n");

		if (body == NULL) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() can't find body\n");
			e->event.failure = GG_FAILURE_CONNECTING;
			return GG_ACTION_FAIL;
		} else {
			body += 2;
		}
	} else {
		body += 4;
	}

	if (res != 0 && body == NULL)
		return GG_ACTION_WAIT;

	gg_debug_session(sess, GG_DEBUG_TRAFFIC, "// received proxy reply:\n%s\n", sess->recv_buf);

	res = sscanf(sess->recv_buf, "HTTP/1.%*d %3d ", &reply);

	gg_debug_session(sess, GG_DEBUG_MISC, "res = %d, reply = %d\n", res, reply);

	/* sprawdzamy, czy wszystko w porządku. */
	if (res != 1 || reply != 200) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() invalid http reply, connection failed\n");
		e->event.failure = GG_FAILURE_CONNECTING;
		return GG_ACTION_FAIL;
	}

	if (body == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() can't find body\n");
		e->event.failure = GG_FAILURE_CONNECTING;
		return GG_ACTION_FAIL;
	}

	gg_debug_session(sess, GG_DEBUG_MISC, "// found body!\n");

	if (sess->ssl_flag != GG_SSL_DISABLED) {
		if (gg_session_init_ssl(sess) == -1) {
			e->event.failure = GG_FAILURE_TLS;
			return GG_ACTION_FAIL;
		}

		/* Teoretycznie SSL jest inicjowany przez klienta, więc serwer
		 * nie powinien niczego wysłać. */
		if (sess->recv_buf + sess->recv_done > body) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() unexpected SSL data\n");
			e->event.failure = GG_FAILURE_TLS;
			return GG_ACTION_FAIL;
		}

		free(sess->recv_buf);
		sess->recv_buf = NULL;
		sess->recv_done = 0;

		sess->state = alt_state;
		sess->check = GG_CHECK_WRITE;
		sess->timeout = GG_DEFAULT_TIMEOUT;

		return GG_ACTION_WAIT;
	}

	sess->state = next_state;
	sess->check = GG_CHECK_READ;
	sess->timeout = GG_DEFAULT_TIMEOUT;	/* Pierwszy pakiet musi przyjść */

	// Jeśli zbuforowaliśmy za dużo, przeanalizuj

	if (sess->recv_buf + sess->recv_done > body) {
		sess->recv_done = sess->recv_done - (body - sess->recv_buf);
		memmove(sess->recv_buf, body, sess->recv_done);
		sess->state = alt2_state;
		return GG_ACTION_NEXT;
	} else {
		free(sess->recv_buf);
		sess->recv_buf = NULL;
		sess->recv_done = 0;
	}

	return GG_ACTION_WAIT;
}