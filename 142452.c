static void hgcm_call_add_pagelist_size(void *buf, u32 len, size_t *extra)
{
	u32 page_count;

	page_count = hgcm_call_buf_size_in_pages(buf, len);
	*extra += offsetof(struct vmmdev_hgcm_pagelist, pages[page_count]);
}