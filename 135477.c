static int proc_sys_getattr(const struct path *path, struct kstat *stat,
			    u32 request_mask, unsigned int query_flags)
{
	struct inode *inode = d_inode(path->dentry);
	struct ctl_table_header *head = grab_header(inode);
	struct ctl_table *table = PROC_I(inode)->sysctl_entry;

	if (IS_ERR(head))
		return PTR_ERR(head);

	generic_fillattr(inode, stat);
	if (table)
		stat->mode = (stat->mode & S_IFMT) | table->mode;

	sysctl_head_finish(head);
	return 0;
}