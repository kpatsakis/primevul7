usm_clone(netsnmp_pdu *pdu, netsnmp_pdu *new_pdu)
{
    struct usmStateReference *ref = pdu->securityStateRef;
    struct usmStateReference **new_ref =
        (struct usmStateReference **)&new_pdu->securityStateRef;
    int ret = 0;

    if (!ref)
        return ret;

    if (pdu->command == SNMP_MSG_TRAP2) {
        netsnmp_assert(pdu->securityModel == SNMP_DEFAULT_SECMODEL);
        ret = usm_clone_usmStateReference(ref, new_ref);
    } else {
        netsnmp_assert(ref == *new_ref);
        ref->refcnt++;
    }

    return ret;
}