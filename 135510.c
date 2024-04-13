static __poll_t proc_sys_poll(struct file *filp, poll_table *wait)
{
	struct inode *inode = file_inode(filp);
	struct ctl_table_header *head = grab_header(inode);
	struct ctl_table *table = PROC_I(inode)->sysctl_entry;
	__poll_t ret = DEFAULT_POLLMASK;
	unsigned long event;

	/* sysctl was unregistered */
	if (IS_ERR(head))
		return EPOLLERR | EPOLLHUP;

	if (!table->proc_handler)
		goto out;

	if (!table->poll)
		goto out;

	event = (unsigned long)filp->private_data;
	poll_wait(filp, &table->poll->wait, wait);

	if (event != atomic_read(&table->poll->event)) {
		filp->private_data = proc_sys_poll_event(table->poll);
		ret = EPOLLIN | EPOLLRDNORM | EPOLLERR | EPOLLPRI;
	}

out:
	sysctl_head_finish(head);

	return ret;
}