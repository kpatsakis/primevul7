recvmsg_blocking(void *data)
{
    struct iomsg_arg *arg = data;
    int flags = 0;
    return rsock_recvmsg(arg->fd, &arg->msg, flags);
}