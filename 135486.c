static bool proc_sys_link_fill_cache(struct file *file,
				    struct dir_context *ctx,
				    struct ctl_table_header *head,
				    struct ctl_table *table)
{
	bool ret = true;

	head = sysctl_head_grab(head);
	if (IS_ERR(head))
		return false;

	/* It is not an error if we can not follow the link ignore it */
	if (sysctl_follow_link(&head, &table))
		goto out;

	ret = proc_sys_fill_cache(file, ctx, head, table);
out:
	sysctl_head_finish(head);
	return ret;
}