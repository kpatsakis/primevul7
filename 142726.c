usm_set_usmStateReference_auth_key(struct usmStateReference *ref,
                                   u_char * auth_key, size_t auth_key_len)
{
    MAKE_ENTRY(ref, u_char, auth_key, auth_key_len,
               usr_auth_key, usr_auth_key_length);
}