static int vbg_hgcm_do_call(struct vbg_dev *gdev, struct vmmdev_hgcm_call *call,
			    u32 timeout_ms, bool *leak_it)
{
	int rc, cancel_rc, ret;
	long timeout;

	*leak_it = false;

	rc = vbg_req_perform(gdev, call);

	/*
	 * If the call failed, then pretend success. Upper layers will
	 * interpret the result code in the packet.
	 */
	if (rc < 0) {
		call->header.result = rc;
		return 0;
	}

	if (rc != VINF_HGCM_ASYNC_EXECUTE)
		return 0;

	/* Host decided to process the request asynchronously, wait for it */
	if (timeout_ms == U32_MAX)
		timeout = MAX_SCHEDULE_TIMEOUT;
	else
		timeout = msecs_to_jiffies(timeout_ms);

	timeout = wait_event_interruptible_timeout(
					gdev->hgcm_wq,
					hgcm_req_done(gdev, &call->header),
					timeout);

	/* timeout > 0 means hgcm_req_done has returned true, so success */
	if (timeout > 0)
		return 0;

	if (timeout == 0)
		ret = -ETIMEDOUT;
	else
		ret = -EINTR;

	/* Cancel the request */
	cancel_rc = hgcm_cancel_call(gdev, call);
	if (cancel_rc >= 0)
		return ret;

	/*
	 * Failed to cancel, this should mean that the cancel has lost the
	 * race with normal completion, wait while the host completes it.
	 */
	if (cancel_rc == VERR_NOT_FOUND || cancel_rc == VERR_SEM_DESTROYED)
		timeout = msecs_to_jiffies(500);
	else
		timeout = msecs_to_jiffies(2000);

	timeout = wait_event_timeout(gdev->hgcm_wq,
				     hgcm_req_done(gdev, &call->header),
				     timeout);

	if (WARN_ON(timeout == 0)) {
		/* We really should never get here */
		vbg_err("%s: Call timedout and cancellation failed, leaking the request\n",
			__func__);
		*leak_it = true;
		return ret;
	}

	/* The call has completed normally after all */
	return 0;
}