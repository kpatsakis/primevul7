sendmsg_blocking(void *data)
{
    struct iomsg_arg *arg = data;
    return sendmsg(arg->fd, &arg->msg, 0);
}