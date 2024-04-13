usm_set_usmStateReference_priv_protocol(struct usmStateReference *ref,
                                        oid * priv_protocol,
                                        size_t priv_protocol_len)
{
    MAKE_ENTRY(ref, oid, priv_protocol, priv_protocol_len,
               usr_priv_protocol, usr_priv_protocol_length);
}