get_new_scriptitem(int *error)
{
    static scid_T   last_current_SID = 0;
    int		    sid = ++last_current_SID;
    scriptitem_T    *si = NULL;

    if (ga_grow(&script_items, (int)(sid - script_items.ga_len)) == FAIL)
    {
	*error = FAIL;
	return sid;
    }
    while (script_items.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	if (si == NULL)
	{
	    *error = FAIL;
	    return sid;
	}
	++script_items.ga_len;
	SCRIPT_ITEM(script_items.ga_len) = si;
	si->sn_name = NULL;
	si->sn_version = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_items.ga_len);
	ga_init2(&si->sn_var_vals, sizeof(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, sizeof(imported_T), 10);
	ga_init2(&si->sn_type_list, sizeof(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    if (si != NULL)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    return sid;
}