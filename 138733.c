unix_path(VALUE sock)
{
    rb_io_t *fptr;

    GetOpenFile(sock, fptr);
    if (NIL_P(fptr->pathv)) {
	struct sockaddr_un addr;
	socklen_t len = (socklen_t)sizeof(addr);
	socklen_t len0 = len;
	if (getsockname(fptr->fd, (struct sockaddr*)&addr, &len) < 0)
            rsock_sys_fail_path("getsockname(2)", fptr->pathv);
        if (len0 < len) len = len0;
	fptr->pathv = rb_obj_freeze(rsock_unixpath_str(&addr, len));
    }
    return rb_str_dup(fptr->pathv);
}