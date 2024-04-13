static void uprobe_warn(struct task_struct *t, const char *msg)
{
	pr_warn("uprobe: %s:%d failed to %s\n",
			current->comm, current->pid, msg);
}