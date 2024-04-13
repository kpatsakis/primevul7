static inline bool consumer_filter(struct uprobe_consumer *uc,
				   enum uprobe_filter_ctx ctx, struct mm_struct *mm)
{
	return !uc->filter || uc->filter(uc, ctx, mm);
}