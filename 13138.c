copy_acls(struct archive_write_disk *a, int tmpfd, int dffd)
{
#ifndef HAVE_SYS_ACL_H
	return 0;
#else
	acl_t acl, dfacl = NULL;
	int acl_r, ret = ARCHIVE_OK;

	acl = acl_get_fd(tmpfd);
	if (acl == NULL) {
		if (errno == ENOENT)
			/* There are not any ACLs. */
			return (ret);
		archive_set_error(&a->archive, errno,
		    "Failed to get metadata(acl)");
		ret = ARCHIVE_WARN;
		goto exit_acl;
	}
	dfacl = acl_dup(acl);
	acl_r = acl_set_fd(dffd, dfacl);
	if (acl_r == -1) {
		archive_set_error(&a->archive, errno,
		    "Failed to get metadata(acl)");
		ret = ARCHIVE_WARN;
		goto exit_acl;
	}
exit_acl:
	if (acl)
		acl_free(acl);
	if (dfacl)
		acl_free(dfacl);
	return (ret);
#endif
}