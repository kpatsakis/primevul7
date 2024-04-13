static struct pai_val *load_inherited_info(const struct connection_struct *conn,
					   const char *fname)
{
	char *pai_buf;
	size_t pai_buf_size = 1024;
	struct pai_val *paiv = NULL;
	ssize_t ret;

	if (!lp_map_acl_inherit(SNUM(conn))) {
		return NULL;
	}

	if ((pai_buf = (char *)SMB_MALLOC(pai_buf_size)) == NULL) {
		return NULL;
	}

	do {
		ret = SMB_VFS_GETXATTR(conn, fname,
				       SAMBA_POSIX_INHERITANCE_EA_NAME,
				       pai_buf, pai_buf_size);

		if (ret == -1) {
			if (errno != ERANGE) {
				break;
			}
			/* Buffer too small - enlarge it. */
			pai_buf_size *= 2;
			SAFE_FREE(pai_buf);
			if (pai_buf_size > 1024*1024) {
				return NULL; /* Limit malloc to 1mb. */
			}
			if ((pai_buf = (char *)SMB_MALLOC(pai_buf_size)) == NULL)
				return NULL;
		}
	} while (ret == -1);

	DEBUG(10,("load_inherited_info: ret = %lu for file %s\n", (unsigned long)ret, fname));

	if (ret == -1) {
		/* No attribute or not supported. */
#if defined(ENOATTR)
		if (errno != ENOATTR)
			DEBUG(10,("load_inherited_info: Error %s\n", strerror(errno) ));
#else
		if (errno != ENOSYS)
			DEBUG(10,("load_inherited_info: Error %s\n", strerror(errno) ));
#endif
		SAFE_FREE(pai_buf);
		return NULL;
	}

	paiv = create_pai_val(pai_buf, ret);

	if (paiv && paiv->pai_protected) {
		DEBUG(10,("load_inherited_info: ACL is protected for file %s\n", fname));
	}

	SAFE_FREE(pai_buf);
	return paiv;
}