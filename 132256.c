nfs_readdir_page_array_alloc(u64 last_cookie, gfp_t gfp_flags)
{
	struct page *page = alloc_page(gfp_flags);
	if (page)
		nfs_readdir_page_init_array(page, last_cookie);
	return page;
}