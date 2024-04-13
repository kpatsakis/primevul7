usm_set_usmStateReference_name(struct usmStateReference *ref,
                               char *name, size_t name_len)
{
    MAKE_ENTRY(ref, char, name, name_len, usr_name, usr_name_length);
}