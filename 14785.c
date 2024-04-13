find_script_callback(char_u *fname, void *cookie)
{
    int sid;
    int error = OK;
    int *ret_sid = cookie;

    sid = find_script_by_name(fname);
    if (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}