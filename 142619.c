usm_add_user_to_list(struct usmUser *user, struct usmUser *puserList)
{
    struct usmUser *nptr, *pptr, *optr;

    /*
     * loop through puserList till we find the proper, sorted place to
     * insert the new user
     */
    /* XXX - how to handle a NULL user->name ?? */
    /* XXX - similarly for a NULL nptr->name ?? */
    for (nptr = puserList, pptr = NULL; nptr != NULL;
         pptr = nptr, nptr = nptr->next) {
        if (nptr->engineIDLen > user->engineIDLen)
            break;

        if (user->engineID == NULL && nptr->engineID != NULL)
            break;

        if (nptr->engineIDLen == user->engineIDLen &&
            (nptr->engineID != NULL && user->engineID != NULL &&
             memcmp(nptr->engineID, user->engineID,
                    user->engineIDLen) > 0))
            break;

        if (!(nptr->engineID == NULL && user->engineID != NULL)) {
            if (nptr->engineIDLen == user->engineIDLen &&
                ((nptr->engineID == NULL && user->engineID == NULL) ||
                 memcmp(nptr->engineID, user->engineID,
                        user->engineIDLen) == 0)
                && strlen(nptr->name) > strlen(user->name))
                break;

            if (nptr->engineIDLen == user->engineIDLen &&
                ((nptr->engineID == NULL && user->engineID == NULL) ||
                 memcmp(nptr->engineID, user->engineID,
                        user->engineIDLen) == 0)
                && strlen(nptr->name) == strlen(user->name)
                && strcmp(nptr->name, user->name) > 0)
                break;

            if (nptr->engineIDLen == user->engineIDLen &&
                ((nptr->engineID == NULL && user->engineID == NULL) ||
                 memcmp(nptr->engineID, user->engineID,
                        user->engineIDLen) == 0)
                && strlen(nptr->name) == strlen(user->name)
                && strcmp(nptr->name, user->name) == 0) {
                /*
                 * the user is an exact match of a previous entry.
                 * Credentials may be different, though, so remove
                 * the old entry (and add the new one)!
                 */
                if (pptr) { /* change prev's next pointer */
                  pptr->next = nptr->next;
                }
                if (nptr->next) { /* change next's prev pointer */
                  nptr->next->prev = pptr;
                }
                optr = nptr;
                nptr = optr->next; /* add new user at this position */
                /* free the old user */
                optr->next=NULL;
                optr->prev=NULL;
                usm_free_user(optr);
                break; /* new user will be added below */
            }
        }
    }

    /*
     * nptr should now point to the user that we need to add ourselves
     * in front of, and pptr should be our new 'prev'.
     */

    /*
     * change our pointers
     */
    user->prev = pptr;
    user->next = nptr;

    /*
     * change the next's prev pointer
     */
    if (user->next)
        user->next->prev = user;

    /*
     * change the prev's next pointer
     */
    if (user->prev)
        user->prev->next = user;

    /*
     * rewind to the head of the list and return it (since the new head
     * could be us, we need to notify the above routine who the head now is.
     */
    for (pptr = user; pptr->prev != NULL; pptr = pptr->prev);
    return pptr;
}