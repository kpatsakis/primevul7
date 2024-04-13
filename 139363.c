static gg_action_t gg_handle_send_hub(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	char *req, *client, *auth;
	const char *host;
	int res;
	int proxy;
	size_t req_len;

	if (sess->client_version != NULL && isdigit(sess->client_version[0]))
		client = gg_urlencode(sess->client_version);
	else
		client = gg_urlencode(GG_DEFAULT_CLIENT_VERSION);

	if (client == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() out of memory for client version\n");
		return GG_ACTION_FAIL;
	}

	if (sess->proxy_addr && sess->proxy_port) {
		host = "http://" GG_APPMSG_HOST;
		proxy = 1;
	} else {
		host = "";
		proxy = 0;
	}

	auth = gg_proxy_auth();

	if (sess->ssl_flag != GG_SSL_DISABLED) {
		req = gg_saprintf
			("GET %s/appsvc/appmsg_ver10.asp?fmnumber=%u&fmt=2&lastmsg=%d&version=%s&age=2&gender=1 HTTP/1.0\r\n"
			"Connection: close\r\n"
			"Host: " GG_APPMSG_HOST "\r\n"
			"%s"
			"\r\n", host, sess->uin, sess->last_sysmsg, client, (auth) ? auth : "");
	} else {
		req = gg_saprintf
			("GET %s/appsvc/appmsg_ver8.asp?fmnumber=%u&fmt=2&lastmsg=%d&version=%s HTTP/1.0\r\n"
			"Host: " GG_APPMSG_HOST "\r\n"
			"%s"
			"\r\n", host, sess->uin, sess->last_sysmsg, client, (auth) ? auth : "");
	}

	if (req == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() out of memory\n");
		e->event.failure = GG_FAILURE_PROXY;
		return GG_ACTION_FAIL;
	}

	req_len = strlen(req);

	free(auth);
	free(client);

	gg_debug_session(sess, GG_DEBUG_TRAFFIC, "// sending http query:\n%s", req);

	res = send(sess->fd, req, req_len, 0);

	free(req);
	
	if (res == -1 && errno != EINTR && errno != EAGAIN) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() sending query failed\n");
		e->event.failure = (!proxy) ? GG_FAILURE_HUB : GG_FAILURE_PROXY;
		return GG_ACTION_FAIL;
	}

	if ((size_t) res < req_len) {
		sess->state = alt_state;
		sess->check = GG_CHECK_WRITE;
		sess->timeout = GG_DEFAULT_TIMEOUT;
	} else {
		sess->state = next_state;
		sess->check = GG_CHECK_READ;
		sess->timeout = GG_DEFAULT_TIMEOUT;
	}

	return GG_ACTION_WAIT;
}