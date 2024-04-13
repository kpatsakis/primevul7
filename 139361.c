static gg_action_t gg_handle_reading_hub_proxy(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	char buf[1024], *tmp, host[128];
	int port = GG_DEFAULT_PORT;
	int reply;
	const char *body;
	struct in_addr addr;
	int res;

	res = recv(sess->fd, buf, sizeof(buf), 0);

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

	if (res != 0)
		return GG_ACTION_WAIT;

	gg_debug_session(sess, GG_DEBUG_TRAFFIC, "// received http reply:\n%s", sess->recv_buf);

	res = sscanf(sess->recv_buf, "HTTP/1.%*d %3d ", &reply);

	/* sprawdzamy, czy wszystko w porządku. */
	if (res != 1 || reply != 200) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() invalid http reply, connection failed\n");
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

	// 17591 0 91.197.13.71:8074 91.197.13.71
	res = sscanf(body, "%d %*d %128s", &reply, host);

	if (res != 2) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() invalid hub reply, connection failed\n");
		e->event.failure = GG_FAILURE_CONNECTING;
		return GG_ACTION_FAIL;
	}

	gg_debug_session(sess, GG_DEBUG_MISC, "reply=%d, host=\"%s\"\n", reply, host);

	/* jeśli pierwsza liczba w linii nie jest równa zeru,
	 * oznacza to, że mamy wiadomość systemową. */
	if (reply != 0) {
		tmp = strchr(body, '\n');

		if (tmp != NULL) {
			e->type = GG_EVENT_MSG;
			e->event.msg.msgclass = reply;
			e->event.msg.sender = 0;
			e->event.msg.message = (unsigned char*) strdup(tmp + 1);

			if (e->event.msg.message == NULL) {
				gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() not enough memory for system message\n");
				return GG_ACTION_FAIL;
			}
		}
	}

	close(sess->fd);
	sess->fd = -1;

	tmp = strchr(host, ':');

	if (tmp != NULL) {
		*tmp = 0;
		port = atoi(tmp + 1);
	}

	if (strcmp(host, "notoperating") == 0) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() service unavailable\n", errno, strerror(errno));
		e->event.failure = GG_FAILURE_UNAVAILABLE;
		return GG_ACTION_FAIL;
	}

	addr.s_addr = inet_addr(host);
	sess->server_addr = addr.s_addr;

	free(sess->recv_buf);
	sess->recv_buf = NULL;
	sess->recv_done = 0;

	if (sess->state != GG_STATE_READING_PROXY_HUB) {
		if (sess->port == 0) {
			sess->connect_port[0] = port;
			sess->connect_port[1] = (port != GG_HTTPS_PORT) ? GG_HTTPS_PORT : 0;
		} else {
			sess->connect_port[0] = sess->port;
			sess->connect_port[1] = 0;
		}
	} else {
		sess->connect_port[0] = (sess->port == 0) ? GG_HTTPS_PORT : sess->port;
		sess->connect_port[1] = 0;
	}

	free(sess->connect_host);
	sess->connect_host = strdup(host);

	if (sess->connect_host == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() not enough memory\n");
		return GG_ACTION_FAIL;
	}

	if (sess->state == GG_STATE_READING_HUB)
		sess->resolver_host = sess->connect_host;

	/* Jeśli łączymy się przez proxy, zacznijmy od początku listy */
	sess->resolver_index = 0;

	sess->state = (sess->async) ? next_state : alt_state;

	return GG_ACTION_NEXT;
}