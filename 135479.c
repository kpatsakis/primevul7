static int proc_sys_readdir(struct file *file, struct dir_context *ctx)
{
	struct ctl_table_header *head = grab_header(file_inode(file));
	struct ctl_table_header *h = NULL;
	struct ctl_table *entry;
	struct ctl_dir *ctl_dir;
	unsigned long pos;

	if (IS_ERR(head))
		return PTR_ERR(head);

	ctl_dir = container_of(head, struct ctl_dir, header);

	if (!dir_emit_dots(file, ctx))
		goto out;

	pos = 2;

	for (first_entry(ctl_dir, &h, &entry); h; next_entry(&h, &entry)) {
		if (!scan(h, entry, &pos, file, ctx)) {
			sysctl_head_finish(h);
			break;
		}
	}
out:
	sysctl_head_finish(head);
	return 0;
}