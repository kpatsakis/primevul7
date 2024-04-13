nfs_readdir_page_unlock_and_put_cached(struct nfs_readdir_descriptor *desc)
{
	unlock_page(desc->page);
	nfs_readdir_page_put(desc);
}