static int insert_entry(struct ctl_table_header *head, struct ctl_table *entry)
{
	struct rb_node *node = &head->node[entry - head->ctl_table].node;
	struct rb_node **p = &head->parent->root.rb_node;
	struct rb_node *parent = NULL;
	const char *name = entry->procname;
	int namelen = strlen(name);

	while (*p) {
		struct ctl_table_header *parent_head;
		struct ctl_table *parent_entry;
		struct ctl_node *parent_node;
		const char *parent_name;
		int cmp;

		parent = *p;
		parent_node = rb_entry(parent, struct ctl_node, node);
		parent_head = parent_node->header;
		parent_entry = &parent_head->ctl_table[parent_node - parent_head->node];
		parent_name = parent_entry->procname;

		cmp = namecmp(name, namelen, parent_name, strlen(parent_name));
		if (cmp < 0)
			p = &(*p)->rb_left;
		else if (cmp > 0)
			p = &(*p)->rb_right;
		else {
			pr_err("sysctl duplicate entry: ");
			sysctl_print_dir(head->parent);
			pr_cont("/%s\n", entry->procname);
			return -EEXIST;
		}
	}

	rb_link_node(node, parent, p);
	rb_insert_color(node, &head->parent->root);
	return 0;
}