static void print_canon_ace(canon_ace *pace, int num)
{
	dbgtext( "canon_ace index %d. Type = %s ", num, pace->attr == ALLOW_ACE ? "allow" : "deny" );
	dbgtext( "SID = %s ", sid_string_dbg(&pace->trustee));
	if (pace->owner_type == UID_ACE) {
		const char *u_name = uidtoname(pace->unix_ug.uid);
		dbgtext( "uid %u (%s) ", (unsigned int)pace->unix_ug.uid, u_name );
	} else if (pace->owner_type == GID_ACE) {
		char *g_name = gidtoname(pace->unix_ug.gid);
		dbgtext( "gid %u (%s) ", (unsigned int)pace->unix_ug.gid, g_name );
	} else
		dbgtext( "other ");
	switch (pace->type) {
		case SMB_ACL_USER:
			dbgtext( "SMB_ACL_USER ");
			break;
		case SMB_ACL_USER_OBJ:
			dbgtext( "SMB_ACL_USER_OBJ ");
			break;
		case SMB_ACL_GROUP:
			dbgtext( "SMB_ACL_GROUP ");
			break;
		case SMB_ACL_GROUP_OBJ:
			dbgtext( "SMB_ACL_GROUP_OBJ ");
			break;
		case SMB_ACL_OTHER:
			dbgtext( "SMB_ACL_OTHER ");
			break;
		default:
			dbgtext( "MASK " );
			break;
	}
	if (pace->inherited)
		dbgtext( "(inherited) ");
	dbgtext( "perms ");
	dbgtext( "%c", pace->perms & S_IRUSR ? 'r' : '-');
	dbgtext( "%c", pace->perms & S_IWUSR ? 'w' : '-');
	dbgtext( "%c\n", pace->perms & S_IXUSR ? 'x' : '-');
}