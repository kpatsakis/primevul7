static struct return_instance *free_ret_instance(struct return_instance *ri)
{
	struct return_instance *next = ri->next;
	put_uprobe(ri->uprobe);
	kfree(ri);
	return next;
}