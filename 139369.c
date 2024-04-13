static int gg_async_connect_failed(struct gg_session *gs, int *res_ptr)
{
	int res = 0;
	socklen_t res_size = sizeof(res);

	if (!gs->async)
		return 0;

	if (gs->timeout == 0) {
		*res_ptr = ETIMEDOUT;
		return 1;
	}

	if (getsockopt(gs->fd, SOL_SOCKET, SO_ERROR, &res, &res_size) == -1) {
		*res_ptr = errno;
		return 1;
	}

	if (res != 0) {
		*res_ptr = res;
		return 1;
	}

	*res_ptr = 0;

	return 0;
}