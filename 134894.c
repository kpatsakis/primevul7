STATIC void
S_delete_recursion_entry(pTHX_ void *key)
{
    /* Deletes the entry used to detect recursion when expanding user-defined
     * properties.  This is a function so it can be set up to be called even if
     * the program unexpectedly quits */

    dVAR;
    SV ** current_entry;
    const STRLEN key_len = strlen((const char *) key);
    DECLARATION_FOR_GLOBAL_CONTEXT;

    SWITCH_TO_GLOBAL_CONTEXT;

    /* If the entry is one of these types, it is a permanent entry, and not the
     * one used to detect recursions.  This function should delete only the
     * recursion entry */
    current_entry = hv_fetch(PL_user_def_props, (const char *) key, key_len, 0);
    if (     current_entry
        && ! is_invlist(*current_entry)
        && ! SvPOK(*current_entry))
    {
        (void) hv_delete(PL_user_def_props, (const char *) key, key_len,
                                                                    G_DISCARD);
    }

    RESTORE_CONTEXT;