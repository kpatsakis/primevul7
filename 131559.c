static SEC_ACCESS map_canon_ace_perms(int snum,
				enum security_ace_type *pacl_type,
				mode_t perms,
				bool directory_ace)
{
	SEC_ACCESS sa;
	uint32 nt_mask = 0;

	*pacl_type = SEC_ACE_TYPE_ACCESS_ALLOWED;

	if (lp_acl_map_full_control(snum) && ((perms & ALL_ACE_PERMS) == ALL_ACE_PERMS)) {
		if (directory_ace) {
			nt_mask = UNIX_DIRECTORY_ACCESS_RWX;
		} else {
			nt_mask = UNIX_ACCESS_RWX;
		}
	} else if ((perms & ALL_ACE_PERMS) == (mode_t)0) {
		/*
		 * Windows NT refuses to display ACEs with no permissions in them (but
		 * they are perfectly legal with Windows 2000). If the ACE has empty
		 * permissions we cannot use 0, so we use the otherwise unused
		 * WRITE_OWNER permission, which we ignore when we set an ACL.
		 * We abstract this into a #define of UNIX_ACCESS_NONE to allow this
		 * to be changed in the future.
		 */

		if (nt4_compatible_acls())
			nt_mask = UNIX_ACCESS_NONE;
		else
			nt_mask = 0;
	} else {
		if (directory_ace) {
			nt_mask |= ((perms & S_IRUSR) ? UNIX_DIRECTORY_ACCESS_R : 0 );
			nt_mask |= ((perms & S_IWUSR) ? UNIX_DIRECTORY_ACCESS_W : 0 );
			nt_mask |= ((perms & S_IXUSR) ? UNIX_DIRECTORY_ACCESS_X : 0 );
		} else {
			nt_mask |= ((perms & S_IRUSR) ? UNIX_ACCESS_R : 0 );
			nt_mask |= ((perms & S_IWUSR) ? UNIX_ACCESS_W : 0 );
			nt_mask |= ((perms & S_IXUSR) ? UNIX_ACCESS_X : 0 );
		}
	}

	DEBUG(10,("map_canon_ace_perms: Mapped (UNIX) %x to (NT) %x\n",
			(unsigned int)perms, (unsigned int)nt_mask ));

	init_sec_access(&sa,nt_mask);
	return sa;
}