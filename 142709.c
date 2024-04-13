usm_clone_usmStateReference(struct usmStateReference *from, struct usmStateReference **to)
{
    struct usmStateReference *cloned_usmStateRef;

    if (from == NULL || to == NULL)
        return -1;

    *to = usm_malloc_usmStateReference();
    cloned_usmStateRef = *to;

    if (usm_set_usmStateReference_name(cloned_usmStateRef, from->usr_name, from->usr_name_length) ||
        usm_set_usmStateReference_engine_id(cloned_usmStateRef, from->usr_engine_id, from->usr_engine_id_length) ||
        usm_set_usmStateReference_auth_protocol(cloned_usmStateRef, from->usr_auth_protocol, from->usr_auth_protocol_length) ||
        usm_set_usmStateReference_auth_key(cloned_usmStateRef, from->usr_auth_key, from->usr_auth_key_length) ||
        usm_set_usmStateReference_priv_protocol(cloned_usmStateRef, from->usr_priv_protocol, from->usr_priv_protocol_length) ||
        usm_set_usmStateReference_priv_key(cloned_usmStateRef, from->usr_priv_key, from->usr_priv_key_length) ||
        usm_set_usmStateReference_sec_level(cloned_usmStateRef, from->usr_sec_level))
    {
        usm_free_usmStateReference(*to);
        *to = NULL;
        return -1;
    }

    return 0;

}