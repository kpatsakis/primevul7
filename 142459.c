void *vbg_req_alloc(size_t len, enum vmmdev_request_type req_type,
		    u32 requestor)
{
	struct vmmdev_request_header *req;
	int order = get_order(PAGE_ALIGN(len));

	req = (void *)__get_free_pages(GFP_KERNEL | GFP_DMA32, order);
	if (!req)
		return NULL;

	memset(req, 0xaa, len);

	req->size = len;
	req->version = VMMDEV_REQUEST_HEADER_VERSION;
	req->request_type = req_type;
	req->rc = VERR_GENERAL_FAILURE;
	req->reserved1 = 0;
	req->requestor = requestor;

	return req;
}