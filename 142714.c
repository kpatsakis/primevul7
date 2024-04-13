usm_set_usmStateReference_sec_level(struct usmStateReference *ref,
                                    int sec_level)
{
    if (ref == NULL)
        return -1;
    ref->usr_sec_level = sec_level;
    return 0;
}