nfs_readdir_page_get_cached(struct nfs_readdir_descriptor *desc)
{
	return nfs_readdir_page_get_locked(desc->file->f_mapping,
					   desc->page_index,
					   desc->last_cookie);
}