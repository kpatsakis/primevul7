int try_chown(connection_struct *conn, const char *fname, uid_t uid, gid_t gid)
{
	int ret;
	files_struct *fsp;
	SMB_STRUCT_STAT st;

	if(!CAN_WRITE(conn)) {
		return -1;
	}

	/* Case (1). */
	/* try the direct way first */
	ret = SMB_VFS_CHOWN(conn, fname, uid, gid);
	if (ret == 0)
		return 0;

	/* Case (2) / (3) */
	if (lp_enable_privileges()) {

		bool has_take_ownership_priv = user_has_privileges(current_user.nt_user_token,
							      &se_take_ownership);
		bool has_restore_priv = user_has_privileges(current_user.nt_user_token,
						       &se_restore);

		/* Case (2) */
		if ( ( has_take_ownership_priv && ( uid == current_user.ut.uid ) ) ||
		/* Case (3) */
		     ( has_restore_priv ) ) {

			become_root();
			/* Keep the current file gid the same - take ownership doesn't imply group change. */
			ret = SMB_VFS_CHOWN(conn, fname, uid, (gid_t)-1);
			unbecome_root();
			return ret;
		}
	}

	/* Case (4). */
	if (!lp_dos_filemode(SNUM(conn))) {
		errno = EPERM;
		return -1;
	}

	/* only allow chown to the current user. This is more secure,
	   and also copes with the case where the SID in a take ownership ACL is
	   a local SID on the users workstation
	*/
	if (uid != current_user.ut.uid) {
		errno = EPERM;
		return -1;
	}

	if (SMB_VFS_STAT(conn,fname,&st)) {
		return -1;
	}

	if (!NT_STATUS_IS_OK(open_file_fchmod(conn,fname,&st,&fsp))) {
		return -1;
	}

	become_root();
	/* Keep the current file gid the same. */
	ret = SMB_VFS_FCHOWN(fsp, uid, (gid_t)-1);
	unbecome_root();

	close_file_fchmod(fsp);

	return ret;
}