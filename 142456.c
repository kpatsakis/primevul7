int vbg_hgcm_connect(struct vbg_dev *gdev, u32 requestor,
		     struct vmmdev_hgcm_service_location *loc,
		     u32 *client_id, int *vbox_status)
{
	struct vmmdev_hgcm_connect *hgcm_connect = NULL;
	int rc;

	hgcm_connect = vbg_req_alloc(sizeof(*hgcm_connect),
				     VMMDEVREQ_HGCM_CONNECT, requestor);
	if (!hgcm_connect)
		return -ENOMEM;

	hgcm_connect->header.flags = 0;
	memcpy(&hgcm_connect->loc, loc, sizeof(*loc));
	hgcm_connect->client_id = 0;

	rc = vbg_req_perform(gdev, hgcm_connect);

	if (rc == VINF_HGCM_ASYNC_EXECUTE)
		wait_event(gdev->hgcm_wq,
			   hgcm_req_done(gdev, &hgcm_connect->header));

	if (rc >= 0) {
		*client_id = hgcm_connect->client_id;
		rc = hgcm_connect->header.result;
	}

	vbg_req_free(hgcm_connect, sizeof(*hgcm_connect));

	*vbox_status = rc;
	return 0;
}