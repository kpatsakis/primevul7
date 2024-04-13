usm_session_init(netsnmp_session *in_session, netsnmp_session *session)
{
    char *cp;
    size_t i;
    
    if (in_session->securityAuthProtoLen > 0) {
        session->securityAuthProto =
            snmp_duplicate_objid(in_session->securityAuthProto,
                                 in_session->securityAuthProtoLen);
        if (session->securityAuthProto == NULL) {
            in_session->s_snmp_errno = SNMPERR_MALLOC;
            return SNMPERR_MALLOC;
        }
    } else if (get_default_authtype(&i) != NULL) {
        session->securityAuthProto =
            snmp_duplicate_objid(get_default_authtype(NULL), i);
        session->securityAuthProtoLen = i;
    }

    if (in_session->securityPrivProtoLen > 0) {
        session->securityPrivProto =
            snmp_duplicate_objid(in_session->securityPrivProto,
                                 in_session->securityPrivProtoLen);
        if (session->securityPrivProto == NULL) {
            in_session->s_snmp_errno = SNMPERR_MALLOC;
            return SNMPERR_MALLOC;
        }
    } else if (get_default_privtype(&i) != NULL) {
        session->securityPrivProto =
            snmp_duplicate_objid(get_default_privtype(NULL), i);
        session->securityPrivProtoLen = i;
    }

    if ((in_session->securityAuthKeyLen <= 0) &&
        ((cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
				     NETSNMP_DS_LIB_AUTHMASTERKEY)))) {
        size_t buflen = sizeof(session->securityAuthKey);
        u_char *tmpp = session->securityAuthKey;
        session->securityAuthKeyLen = 0;
        /* it will be a hex string */
        if (!snmp_hex_to_binary(&tmpp, &buflen,
                                &session->securityAuthKeyLen, 0, cp)) {
            snmp_set_detail("error parsing authentication master key");
            return SNMP_ERR_GENERR;
        }
    } else if ((in_session->securityAuthKeyLen <= 0) &&
               ((cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
                                            NETSNMP_DS_LIB_AUTHPASSPHRASE)) ||
                (cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
                                            NETSNMP_DS_LIB_PASSPHRASE)))) {
        session->securityAuthKeyLen = USM_AUTH_KU_LEN;
        if (generate_Ku(session->securityAuthProto,
                        session->securityAuthProtoLen,
                        (u_char *) cp, strlen(cp),
                        session->securityAuthKey,
                        &session->securityAuthKeyLen) != SNMPERR_SUCCESS) {
            snmp_set_detail
                ("Error generating a key (Ku) from the supplied authentication pass phrase.");
            return SNMP_ERR_GENERR;
        }
    }

    
    if ((in_session->securityPrivKeyLen <= 0) &&
        ((cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
				     NETSNMP_DS_LIB_PRIVMASTERKEY)))) {
        size_t buflen = sizeof(session->securityPrivKey);
        u_char *tmpp = session->securityPrivKey;
        session->securityPrivKeyLen = 0;
        /* it will be a hex string */
        if (!snmp_hex_to_binary(&tmpp, &buflen,
                                &session->securityPrivKeyLen, 0, cp)) {
            snmp_set_detail("error parsing encryption master key");
            return SNMP_ERR_GENERR;
        }
    } else if ((in_session->securityPrivKeyLen <= 0) &&
               ((cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
                                            NETSNMP_DS_LIB_PRIVPASSPHRASE)) ||
                (cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
                                            NETSNMP_DS_LIB_PASSPHRASE)))) {
        session->securityPrivKeyLen = USM_PRIV_KU_LEN;
        if (generate_Ku(session->securityAuthProto,
                        session->securityAuthProtoLen,
                        (u_char *) cp, strlen(cp),
                        session->securityPrivKey,
                        &session->securityPrivKeyLen) != SNMPERR_SUCCESS) {
            snmp_set_detail
                ("Error generating a key (Ku) from the supplied privacy pass phrase.");
            return SNMP_ERR_GENERR;
        }
    }

    return SNMPERR_SUCCESS;
}