static bool binder_worklist_empty_ilocked(struct list_head *list)
{
	return list_empty(list);
}