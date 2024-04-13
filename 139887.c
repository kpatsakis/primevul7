void uprobe_start_dup_mmap(void)
{
	percpu_down_read(&dup_mmap_sem);
}