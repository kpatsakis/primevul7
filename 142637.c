usm_remove_user_from_list(struct usmUser *user,
                          struct usmUser **ppuserList)
{
    int rc = usm_remove_usmUser_from_list(user, ppuserList);
    if (rc != SNMPERR_SUCCESS || NULL == ppuserList)
        return NULL;

    return *ppuserList;
}                               /* end usm_remove_user_from_list() */