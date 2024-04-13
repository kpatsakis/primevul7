usm_save_users_from_list(struct usmUser *puserList, const char *token,
                         const char *type)
{
    struct usmUser *uptr;
    for (uptr = puserList; uptr != NULL; uptr = uptr->next) {
        if (uptr->userStorageType == ST_NONVOLATILE)
            usm_save_user(uptr, token, type);
    }
}