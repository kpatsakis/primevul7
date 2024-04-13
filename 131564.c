int inherit_access_acl(connection_struct *conn, const char *inherit_from_dir,
		       const char *name, mode_t mode)
{
	if (directory_has_default_acl(conn, inherit_from_dir))
		return 0;

	return copy_access_acl(conn, inherit_from_dir, name, mode);
}