psutil_net_if_flags(PyObject *self, PyObject *args) {
    char *nic_name;
    int sock = -1;
    int ret;
    struct ifreq ifr;

    if (! PyArg_ParseTuple(args, "s", &nic_name))
        return NULL;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        goto error;

    strncpy(ifr.ifr_name, nic_name, sizeof(ifr.ifr_name));
    ret = ioctl(sock, SIOCGIFFLAGS, &ifr);
    if (ret == -1)
        goto error;

    close(sock);
    if ((ifr.ifr_flags & IFF_UP) != 0)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);

error:
    if (sock != -1)
        close(sock);
    return PyErr_SetFromErrno(PyExc_OSError);
}