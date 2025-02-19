int vbg_hgcm_call(struct vbg_dev *gdev, u32 requestor, u32 client_id,
		  u32 function, u32 timeout_ms,
		  struct vmmdev_hgcm_function_parameter *parms, u32 parm_count,
		  int *vbox_status)
{
	struct vmmdev_hgcm_call *call;
	void **bounce_bufs = NULL;
	bool leak_it;
	size_t size;
	int i, ret;

	size = sizeof(struct vmmdev_hgcm_call) +
		   parm_count * sizeof(struct vmmdev_hgcm_function_parameter);
	/*
	 * Validate and buffer the parameters for the call. This also increases
	 * call_size with the amount of extra space needed for page lists.
	 */
	ret = hgcm_call_preprocess(parms, parm_count, &bounce_bufs, &size);
	if (ret) {
		/* Even on error bounce bufs may still have been allocated */
		goto free_bounce_bufs;
	}

	call = vbg_req_alloc(size, VMMDEVREQ_HGCM_CALL, requestor);
	if (!call) {
		ret = -ENOMEM;
		goto free_bounce_bufs;
	}

	hgcm_call_init_call(call, client_id, function, parms, parm_count,
			    bounce_bufs);

	ret = vbg_hgcm_do_call(gdev, call, timeout_ms, &leak_it);
	if (ret == 0) {
		*vbox_status = call->header.result;
		ret = hgcm_call_copy_back_result(call, parms, parm_count,
						 bounce_bufs);
	}

	if (!leak_it)
		vbg_req_free(call, size);

free_bounce_bufs:
	if (bounce_bufs) {
		for (i = 0; i < parm_count; i++)
			kvfree(bounce_bufs[i]);
		kfree(bounce_bufs);
	}

	return ret;
}