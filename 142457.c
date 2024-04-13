static void hgcm_call_init_linaddr(struct vmmdev_hgcm_call *call,
	struct vmmdev_hgcm_function_parameter *dst_parm, void *buf, u32 len,
	enum vmmdev_hgcm_function_parameter_type type, u32 *off_extra)
{
	struct vmmdev_hgcm_pagelist *dst_pg_lst;
	struct page *page;
	bool is_vmalloc;
	u32 i, page_count;

	dst_parm->type = type;

	if (len == 0) {
		dst_parm->u.pointer.size = 0;
		dst_parm->u.pointer.u.linear_addr = 0;
		return;
	}

	dst_pg_lst = (void *)call + *off_extra;
	page_count = hgcm_call_buf_size_in_pages(buf, len);
	is_vmalloc = is_vmalloc_addr(buf);

	dst_parm->type = VMMDEV_HGCM_PARM_TYPE_PAGELIST;
	dst_parm->u.page_list.size = len;
	dst_parm->u.page_list.offset = *off_extra;
	dst_pg_lst->flags = hgcm_call_linear_addr_type_to_pagelist_flags(type);
	dst_pg_lst->offset_first_page = (unsigned long)buf & ~PAGE_MASK;
	dst_pg_lst->page_count = page_count;

	for (i = 0; i < page_count; i++) {
		if (is_vmalloc)
			page = vmalloc_to_page(buf);
		else
			page = virt_to_page(buf);

		dst_pg_lst->pages[i] = page_to_phys(page);
		buf += PAGE_SIZE;
	}

	*off_extra += offsetof(struct vmmdev_hgcm_pagelist, pages[page_count]);
}