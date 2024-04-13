static bool filter_chain(struct uprobe *uprobe,
			 enum uprobe_filter_ctx ctx, struct mm_struct *mm)
{
	struct uprobe_consumer *uc;
	bool ret = false;

	down_read(&uprobe->consumer_rwsem);
	for (uc = uprobe->consumers; uc; uc = uc->next) {
		ret = consumer_filter(uc, ctx, mm);
		if (ret)
			break;
	}
	up_read(&uprobe->consumer_rwsem);

	return ret;
}