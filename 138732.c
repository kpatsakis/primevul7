unix_send_io(VALUE sock, VALUE val)
{
    int fd;
    rb_io_t *fptr;
    struct iomsg_arg arg;
    struct iovec vec[1];
    char buf[1];

#if FD_PASSING_BY_MSG_CONTROL
    union {
	struct cmsghdr hdr;
	char pad[sizeof(struct cmsghdr)+8+sizeof(int)+8];
    } cmsg;
#endif

    if (rb_obj_is_kind_of(val, rb_cIO)) {
        rb_io_t *valfptr;
	GetOpenFile(val, valfptr);
	fd = valfptr->fd;
    }
    else if (FIXNUM_P(val)) {
        fd = FIX2INT(val);
    }
    else {
	rb_raise(rb_eTypeError, "neither IO nor file descriptor");
    }

    GetOpenFile(sock, fptr);

    arg.msg.msg_name = NULL;
    arg.msg.msg_namelen = 0;

    /* Linux and Solaris doesn't work if msg_iov is NULL. */
    buf[0] = '\0';
    vec[0].iov_base = buf;
    vec[0].iov_len = 1;
    arg.msg.msg_iov = vec;
    arg.msg.msg_iovlen = 1;

#if FD_PASSING_BY_MSG_CONTROL
    arg.msg.msg_control = (caddr_t)&cmsg;
    arg.msg.msg_controllen = (socklen_t)CMSG_LEN(sizeof(int));
    arg.msg.msg_flags = 0;
    MEMZERO((char*)&cmsg, char, sizeof(cmsg));
    cmsg.hdr.cmsg_len = (socklen_t)CMSG_LEN(sizeof(int));
    cmsg.hdr.cmsg_level = SOL_SOCKET;
    cmsg.hdr.cmsg_type = SCM_RIGHTS;
    memcpy(CMSG_DATA(&cmsg.hdr), &fd, sizeof(int));
#else
    arg.msg.msg_accrights = (caddr_t)&fd;
    arg.msg.msg_accrightslen = sizeof(fd);
#endif

    arg.fd = fptr->fd;
    while ((int)BLOCKING_REGION_FD(sendmsg_blocking, &arg) == -1) {
	if (!rb_io_wait_writable(arg.fd))
	    rsock_sys_fail_path("sendmsg(2)", fptr->pathv);
    }

    return Qnil;
}