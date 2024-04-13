find_script_by_name(char_u *name)
{
    int		    sid;
    scriptitem_T    *si;

    for (sid = script_items.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	if (si->sn_name != NULL && fnamecmp(si->sn_name, name) == 0)
	    return sid;
    }
    return -1;
}