static struct binder_work *binder_dequeue_work_head_ilocked(
					struct list_head *list)
{
	struct binder_work *w;

	w = list_first_entry_or_null(list, struct binder_work, entry);
	if (w)
		list_del_init(&w->entry);
	return w;
}