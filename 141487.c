psutil_net_if_mtu(PyObject *self, PyObject *args) {
    char *nic_name;
    int sock = -1;
    int ret;
#ifdef PSUTIL_SUNOS10
    struct lifreq lifr;
#else
    struct ifreq ifr;
#endif

    if (! PyArg_ParseTuple(args, "s", &nic_name))
        return NULL;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        goto error;

#ifdef PSUTIL_SUNOS10
    strncpy(lifr.lifr_name, nic_name, sizeof(lifr.lifr_name));
    ret = ioctl(sock, SIOCGIFMTU, &lifr);
#else
    strncpy(ifr.ifr_name, nic_name, sizeof(ifr.ifr_name));
    ret = ioctl(sock, SIOCGIFMTU, &ifr);
#endif
    if (ret == -1)
        goto error;
    close(sock);

#ifdef PSUTIL_SUNOS10
    return Py_BuildValue("i", lifr.lifr_mtu);
#else
    return Py_BuildValue("i", ifr.ifr_mtu);
#endif

error:
    if (sock != -1)
        close(sock);
    return PyErr_SetFromErrno(PyExc_OSError);
}