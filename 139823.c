static int dup_utask(struct task_struct *t, struct uprobe_task *o_utask)
{
	struct uprobe_task *n_utask;
	struct return_instance **p, *o, *n;

	n_utask = kzalloc(sizeof(struct uprobe_task), GFP_KERNEL);
	if (!n_utask)
		return -ENOMEM;
	t->utask = n_utask;

	p = &n_utask->return_instances;
	for (o = o_utask->return_instances; o; o = o->next) {
		n = kmalloc(sizeof(struct return_instance), GFP_KERNEL);
		if (!n)
			return -ENOMEM;

		*n = *o;
		get_uprobe(n->uprobe);
		n->next = NULL;

		*p = n;
		p = &n->next;
		n_utask->depth++;
	}

	return 0;
}