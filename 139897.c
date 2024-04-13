static struct uprobe_task *get_utask(void)
{
	if (!current->utask)
		current->utask = kzalloc(sizeof(struct uprobe_task), GFP_KERNEL);
	return current->utask;
}