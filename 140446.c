static void nested_release_page_clean(struct page *page)
{
	kvm_release_page_clean(page);
}