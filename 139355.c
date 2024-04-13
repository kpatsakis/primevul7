static gg_action_t gg_handle_resolve_sync(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	struct in_addr addr;

	addr.s_addr = inet_addr(sess->resolver_host);
	
	if (addr.s_addr == INADDR_NONE) {
		struct in_addr *addr_list = NULL;
		unsigned int addr_count;

		if (gg_gethostbyname_real(sess->resolver_host, &addr_list, &addr_count, 0) == -1) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() host %s not found\n", sess->resolver_host);
			e->event.failure = GG_FAILURE_RESOLVING;
			free(addr_list);
			return GG_ACTION_FAIL;
		}

		sess->resolver_result = addr_list;
		sess->resolver_count = addr_count;
		sess->resolver_index = 0;
	} else {
		sess->resolver_result = malloc(sizeof(struct in_addr));

		if (sess->resolver_result == NULL) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() out of memory\n");
			return GG_ACTION_FAIL;
		}

		sess->resolver_result[0].s_addr = addr.s_addr;
		sess->resolver_count = 1;
		sess->resolver_index = 0;
	}

	sess->state = next_state;

	return GG_ACTION_NEXT;
}