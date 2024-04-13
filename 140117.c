static void nested_release_page(struct page *page)
{
	kvm_release_page_dirty(page);
}