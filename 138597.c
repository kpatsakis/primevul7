static struct CommandControlBlock *arcmsr_get_freeccb(struct AdapterControlBlock *acb)
{
	struct list_head *head = &acb->ccb_free_list;
	struct CommandControlBlock *ccb = NULL;
	unsigned long flags;
	spin_lock_irqsave(&acb->ccblist_lock, flags);
	if (!list_empty(head)) {
		ccb = list_entry(head->next, struct CommandControlBlock, list);
		list_del_init(&ccb->list);
	}else{
		spin_unlock_irqrestore(&acb->ccblist_lock, flags);
		return NULL;
	}
	spin_unlock_irqrestore(&acb->ccblist_lock, flags);
	return ccb;
}