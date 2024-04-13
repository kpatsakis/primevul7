usm_remove_usmUser_from_list(struct usmUser *user, struct usmUser **ppuserList)
{
    struct usmUser *nptr, *pptr;

    /*
     * NULL pointers aren't allowed
     */
    if (ppuserList == NULL)
        ppuserList = &userList;

    if (*ppuserList == NULL)
        return SNMPERR_USM_UNKNOWNSECURITYNAME;

    /*
     * find the user in the list
     */
    for (nptr = *ppuserList, pptr = NULL; nptr != NULL;
         pptr = nptr, nptr = nptr->next) {
        if (nptr == user)
            break;
    }

    if (nptr) {
        /*
         * remove the user from the linked list
         */
        if (pptr) {
            pptr->next = nptr->next;
        }
        if (nptr->next) {
            nptr->next->prev = pptr;
        }
    } else {
        /*
         * user didn't exist
         */
        return SNMPERR_USM_UNKNOWNSECURITYNAME;
    }
    if (nptr == *ppuserList)    /* we're the head of the list, need to change
                                 * * the head to the next user */
        *ppuserList = nptr->next;
    return SNMPERR_SUCCESS;
}                               /* end usm_remove_usmUser_from_list() */