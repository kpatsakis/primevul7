usm_set_usmStateReference_auth_protocol(struct usmStateReference *ref,
                                        oid * auth_protocol,
                                        size_t auth_protocol_len)
{
    MAKE_ENTRY(ref, oid, auth_protocol, auth_protocol_len,
               usr_auth_protocol, usr_auth_protocol_length);
}