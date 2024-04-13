static gg_action_t gg_handle_resolving(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	char buf[256];
	int count = -1;
	int res;
	unsigned int i;

	res = gg_resolver_recv(sess->fd, buf, sizeof(buf));

	if (res == -1 && (errno == EAGAIN || errno == EINTR)) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() non-critical error (errno=%d, %s)\n", errno, strerror(errno));
		return GG_ACTION_WAIT;
	}

	sess->resolver_cleanup(&sess->resolver, 0);

	if (res == -1) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() read error (errno=%d, %s)\n", errno, strerror(errno));
		e->event.failure = GG_FAILURE_RESOLVING;
		return GG_ACTION_FAIL;
	}

	if (res > 0) {
		char *tmp;

		tmp = realloc(sess->recv_buf, sess->recv_done + res);

		if (tmp == NULL)
			return GG_ACTION_FAIL;

		sess->recv_buf = tmp;
		memcpy(sess->recv_buf + sess->recv_done, buf, res);
		sess->recv_done += res;
	}

	/* Sprawdź, czy mamy listę zakończoną INADDR_NONE */

	for (i = 0; i < sess->recv_done / sizeof(struct in_addr); i++) {
		if (((struct in_addr*) sess->recv_buf)[i].s_addr == INADDR_NONE) {
			count = i;
			break;
		}
	}

	/* Nie znaleziono hosta */

	if (count == 0) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() host not found\n");
		e->event.failure = GG_FAILURE_RESOLVING;
		return GG_ACTION_FAIL;
	}

	/* Nie mamy pełnej listy, ale połączenie zerwane */

	if (res == 0 && count == -1) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() connection broken\n");
		e->event.failure = GG_FAILURE_RESOLVING;
		return GG_ACTION_FAIL;
	}

	/* Nie mamy pełnej listy, normalna sytuacja */

	if (count == -1)
		return GG_ACTION_WAIT;

#ifndef GG_DEBUG_DISABLE
	if ((gg_debug_level & GG_DEBUG_DUMP) && (count > 0)) {
		char *list;
		size_t len;

		len = 0;

		for (i = 0; i < (unsigned int) count; i++) {
			if (i > 0)
				len += 2;

			len += strlen(inet_ntoa(((struct in_addr*) sess->recv_buf)[i]));
		}

		list = malloc(len + 1);

		if (list == NULL)
			return GG_ACTION_FAIL;

		list[0] = 0;

		for (i = 0; i < (unsigned int) count; i++) {
			if (i > 0)
				strcat(list, ", ");

			strcat(list, inet_ntoa(((struct in_addr*) sess->recv_buf)[i]));
		}

		gg_debug_session(sess, GG_DEBUG_DUMP, "// gg_watch_fd() resolved: %s\n", list);

		free(list);
	}
#endif

	close(sess->fd);
	sess->fd = -1;

	sess->state = next_state;
	sess->resolver_result = (struct in_addr*) sess->recv_buf;
	sess->resolver_count = count;
	sess->resolver_index = 0;
	sess->recv_buf = NULL;
	sess->recv_done = 0;
	
	return GG_ACTION_NEXT;
}