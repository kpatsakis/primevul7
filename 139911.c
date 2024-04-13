static void copy_to_page(struct page *page, unsigned long vaddr, const void *src, int len)
{
	void *kaddr = kmap_atomic(page);
	memcpy(kaddr + (vaddr & ~PAGE_MASK), src, len);
	kunmap_atomic(kaddr);
}