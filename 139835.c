static void copy_from_page(struct page *page, unsigned long vaddr, void *dst, int len)
{
	void *kaddr = kmap_atomic(page);
	memcpy(dst, kaddr + (vaddr & ~PAGE_MASK), len);
	kunmap_atomic(kaddr);
}