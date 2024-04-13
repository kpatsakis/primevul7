qemuProcessHandleGraphics(qemuMonitorPtr mon G_GNUC_UNUSED,
                          virDomainObjPtr vm,
                          int phase,
                          int localFamily,
                          const char *localNode,
                          const char *localService,
                          int remoteFamily,
                          const char *remoteNode,
                          const char *remoteService,
                          const char *authScheme,
                          const char *x509dname,
                          const char *saslUsername,
                          void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event;
    virDomainEventGraphicsAddressPtr localAddr = NULL;
    virDomainEventGraphicsAddressPtr remoteAddr = NULL;
    virDomainEventGraphicsSubjectPtr subject = NULL;
    size_t i;

    localAddr = g_new0(virDomainEventGraphicsAddress, 1);
    localAddr->family = localFamily;
    localAddr->service = g_strdup(localService);
    localAddr->node = g_strdup(localNode);

    remoteAddr = g_new0(virDomainEventGraphicsAddress, 1);
    remoteAddr->family = remoteFamily;
    remoteAddr->service = g_strdup(remoteService);
    remoteAddr->node = g_strdup(remoteNode);

    subject = g_new0(virDomainEventGraphicsSubject, 1);
    if (x509dname) {
        if (VIR_REALLOC_N(subject->identities, subject->nidentity+1) < 0)
            goto error;
        subject->nidentity++;
        subject->identities[subject->nidentity - 1].type = g_strdup("x509dname");
        subject->identities[subject->nidentity - 1].name = g_strdup(x509dname);
    }
    if (saslUsername) {
        if (VIR_REALLOC_N(subject->identities, subject->nidentity+1) < 0)
            goto error;
        subject->nidentity++;
        subject->identities[subject->nidentity - 1].type = g_strdup("saslUsername");
        subject->identities[subject->nidentity - 1].name = g_strdup(saslUsername);
    }

    virObjectLock(vm);
    event = virDomainEventGraphicsNewFromObj(vm, phase, localAddr, remoteAddr, authScheme, subject);
    virObjectUnlock(vm);

    virObjectEventStateQueue(driver->domainEventState, event);

    return 0;

 error:
    VIR_FREE(localAddr->service);
    VIR_FREE(localAddr->node);
    VIR_FREE(localAddr);

    VIR_FREE(remoteAddr->service);
    VIR_FREE(remoteAddr->node);
    VIR_FREE(remoteAddr);

    for (i = 0; i < subject->nidentity; i++) {
        VIR_FREE(subject->identities[i].type);
        VIR_FREE(subject->identities[i].name);
    }
    VIR_FREE(subject->identities);
    VIR_FREE(subject);

    return -1;
}