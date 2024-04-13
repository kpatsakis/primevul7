qemuProcessOpenVhostVsock(virDomainVsockDefPtr vsock)
{
    qemuDomainVsockPrivatePtr priv = (qemuDomainVsockPrivatePtr)vsock->privateData;
    const char *vsock_path = "/dev/vhost-vsock";
    int fd;

    if ((fd = open(vsock_path, O_RDWR)) < 0) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       "%s", _("unable to open vhost-vsock device"));
        return -1;
    }

    if (vsock->auto_cid == VIR_TRISTATE_BOOL_YES) {
        if (virVsockAcquireGuestCid(fd, &vsock->guest_cid) < 0)
            goto error;
    } else {
        if (virVsockSetGuestCid(fd, vsock->guest_cid) < 0)
            goto error;
    }

    priv->vhostfd = fd;
    return 0;

 error:
    VIR_FORCE_CLOSE(fd);
    return -1;
}