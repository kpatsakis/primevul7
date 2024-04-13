int chmod_acl(connection_struct *conn, const char *name, mode_t mode)
{
	return copy_access_acl(conn, name, name, mode);
}