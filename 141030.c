static int selinux_inode_setotherxattr(struct dentry *dentry, const char *name)
{
	const struct cred *cred = current_cred();

	if (!strncmp(name, XATTR_SECURITY_PREFIX,
		     sizeof XATTR_SECURITY_PREFIX - 1)) {
		if (!strcmp(name, XATTR_NAME_CAPS)) {
			if (!capable(CAP_SETFCAP))
				return -EPERM;
		} else if (!capable(CAP_SYS_ADMIN)) {
			/* A different attribute in the security namespace.
			   Restrict to administrator. */
			return -EPERM;
		}
	}

	/* Not an attribute we recognize, so just check the
	   ordinary setattr permission. */
	return dentry_has_perm(cred, dentry, FILE__SETATTR);
}