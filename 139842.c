void __weak arch_uprobe_copy_ixol(struct page *page, unsigned long vaddr,
				  void *src, unsigned long len)
{
	/* Initialize the slot */
	copy_to_page(page, vaddr, src, len);

	/*
	 * We probably need flush_icache_user_range() but it needs vma.
	 * This should work on most of architectures by default. If
	 * architecture needs to do something different it can define
	 * its own version of the function.
	 */
	flush_dcache_page(page);
}