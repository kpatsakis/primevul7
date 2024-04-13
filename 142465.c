void vbg_req_free(void *req, size_t len)
{
	if (!req)
		return;

	free_pages((unsigned long)req, get_order(PAGE_ALIGN(len)));
}