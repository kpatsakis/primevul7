static int gg_send_queued_data(struct gg_session *sess)
{
	int res;

	if (sess->send_buf == NULL || sess->send_left == 0)
		return 0;
		
	gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() sending %d bytes of queued data\n", sess->send_left);

	res = send(sess->fd, sess->send_buf, sess->send_left, 0);

	if (res == -1) {
		if (errno == EAGAIN || errno == EINTR) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() non-critical send error (errno=%d, %s)\n", errno, strerror(errno));

			return 0;
		}

		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() send() failed (errno=%d, %s)\n", errno, strerror(errno));

		return -1;
	}

	if (res == sess->send_left) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() sent all queued data\n");
		free(sess->send_buf);
		sess->send_buf = NULL;
		sess->send_left = 0;
	} else if (res > 0) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_watch_fd() sent %d bytes of queued data, %d bytes left\n", res, sess->send_left - res);

		memmove(sess->send_buf, sess->send_buf + res, sess->send_left - res);
		sess->send_left -= res;
	}

	return 0;
}