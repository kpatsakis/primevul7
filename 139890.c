static inline bool uprobe_is_active(struct uprobe *uprobe)
{
	return !RB_EMPTY_NODE(&uprobe->rb_node);
}