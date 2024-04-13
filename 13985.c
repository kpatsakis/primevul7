static int reds_init_socket(const char *addr, int portnr, int family)
{
    static const int on=1, off=0;
    struct addrinfo ai,*res,*e;
    char port[33];
    int slisten, rc;

    if (family == AF_UNIX) {
#ifndef _WIN32
        int len;
        struct sockaddr_un local = { 0, };

        if ((slisten = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            return -1;
        }

        local.sun_family = AF_UNIX;
        g_strlcpy(local.sun_path, addr, sizeof(local.sun_path));
        len = SUN_LEN(&local);
        if (local.sun_path[0] == '@') {
            local.sun_path[0] = 0;
        } else {
            unlink(local.sun_path);
        }
        if (bind(slisten, (struct sockaddr *)&local, len) == -1) {
            perror("bind");
            socket_close(slisten);
            return -1;
        }

        goto listen;
#else
        return -1;
#endif
    }

    memset(&ai,0, sizeof(ai));
    ai.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    ai.ai_socktype = SOCK_STREAM;
    ai.ai_family = family;

    snprintf(port, sizeof(port), "%d", portnr);
    rc = getaddrinfo(strlen(addr) ? addr : NULL, port, &ai, &res);
    if (rc != 0) {
        spice_warning("getaddrinfo(%s,%s): %s", addr, port,
                      gai_strerror(rc));
        return -1;
    }

    for (e = res; e != NULL; e = e->ai_next) {
        slisten = socket(e->ai_family, e->ai_socktype, e->ai_protocol);
        if (slisten < 0) {
            continue;
        }

        setsockopt(slisten,SOL_SOCKET,SO_REUSEADDR,(void*)&on,sizeof(on));
#ifdef IPV6_V6ONLY
        if (e->ai_family == PF_INET6) {
            /* listen on both ipv4 and ipv6 */
            setsockopt(slisten,IPPROTO_IPV6,IPV6_V6ONLY,(void*)&off,
                       sizeof(off));
        }
#endif
        if (bind(slisten, e->ai_addr, e->ai_addrlen) == 0) {
            char uaddr[INET6_ADDRSTRLEN+1];
            char uport[33];
            rc = getnameinfo((struct sockaddr*)e->ai_addr,e->ai_addrlen,
                             uaddr,INET6_ADDRSTRLEN, uport,32,
                             NI_NUMERICHOST | NI_NUMERICSERV);
            if (rc == 0) {
                spice_debug("bound to %s:%s", uaddr, uport);
            } else {
                spice_debug("cannot resolve address spice-server is bound to");
            }
            freeaddrinfo(res);
            goto listen;
        }
        socket_close(slisten);
    }
    spice_warning("binding socket to %s:%d failed", addr, portnr);
    freeaddrinfo(res);
    return -1;

listen:
    if (listen(slisten, SOMAXCONN) != 0) {
        spice_warning("listen: %s", strerror(errno));
        socket_close(slisten);
        return -1;
    }
    return slisten;
}