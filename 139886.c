static int __init init_uprobes(void)
{
	int i;

	for (i = 0; i < UPROBES_HASH_SZ; i++)
		mutex_init(&uprobes_mmap_mutex[i]);

	if (percpu_init_rwsem(&dup_mmap_sem))
		return -ENOMEM;

	return register_die_notifier(&uprobe_exception_nb);
}