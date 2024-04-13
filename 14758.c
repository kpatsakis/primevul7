get_new_scriptitem_for_fname(int *error, char_u *fname)
{
    int sid = get_new_scriptitem(error);

    if (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    return sid;
}