int vbg_hgcm_disconnect(struct vbg_dev *gdev, u32 requestor,
			u32 client_id, int *vbox_status)
{
	struct vmmdev_hgcm_disconnect *hgcm_disconnect = NULL;
	int rc;

	hgcm_disconnect = vbg_req_alloc(sizeof(*hgcm_disconnect),
					VMMDEVREQ_HGCM_DISCONNECT,
					requestor);
	if (!hgcm_disconnect)
		return -ENOMEM;

	hgcm_disconnect->header.flags = 0;
	hgcm_disconnect->client_id = client_id;

	rc = vbg_req_perform(gdev, hgcm_disconnect);

	if (rc == VINF_HGCM_ASYNC_EXECUTE)
		wait_event(gdev->hgcm_wq,
			   hgcm_req_done(gdev, &hgcm_disconnect->header));

	if (rc >= 0)
		rc = hgcm_disconnect->header.result;

	vbg_req_free(hgcm_disconnect, sizeof(*hgcm_disconnect));

	*vbox_status = rc;
	return 0;
}