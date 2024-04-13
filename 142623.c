usm_set_usmStateReference_priv_key(struct usmStateReference *ref,
                                   u_char * priv_key, size_t priv_key_len)
{
    MAKE_ENTRY(ref, u_char, priv_key, priv_key_len,
               usr_priv_key, usr_priv_key_length);
}