usm_set_usmStateReference_engine_id(struct usmStateReference *ref,
                                    u_char * engine_id,
                                    size_t engine_id_len)
{
    MAKE_ENTRY(ref, u_char, engine_id, engine_id_len,
               usr_engine_id, usr_engine_id_length);
}