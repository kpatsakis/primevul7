psutil_net_if_duplex_speed(PyObject *self, PyObject *args) {
    char *nic_name;
    int sock = -1;
    int ret;
    int duplex;
    int speed;
    struct ifreq ifr;
    struct ifmediareq ifmed;

    if (! PyArg_ParseTuple(args, "s", &nic_name))
        return NULL;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        return PyErr_SetFromErrno(PyExc_OSError);
    strncpy(ifr.ifr_name, nic_name, sizeof(ifr.ifr_name));

    // speed / duplex
    memset(&ifmed, 0, sizeof(struct ifmediareq));
    strlcpy(ifmed.ifm_name, nic_name, sizeof(ifmed.ifm_name));
    ret = ioctl(sock, SIOCGIFMEDIA, (caddr_t)&ifmed);
    if (ret == -1) {
        speed = 0;
        duplex = 0;
    }
    else {
        speed = psutil_get_nic_speed(ifmed.ifm_active);
        if ((ifmed.ifm_active | IFM_FDX) == ifmed.ifm_active)
            duplex = 2;
        else if ((ifmed.ifm_active | IFM_HDX) == ifmed.ifm_active)
            duplex = 1;
        else
            duplex = 0;
    }

    close(sock);
    return Py_BuildValue("[ii]", duplex, speed);
}