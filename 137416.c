void reply_ntcancel(struct smb_request *req)
{
	/*
	 * Go through and cancel any pending change notifies.
	 */

	START_PROFILE(SMBntcancel);
	srv_cancel_sign_response(smbd_server_conn);
	remove_pending_change_notify_requests_by_mid(req->mid);
	remove_pending_lock_requests_by_mid(req->mid);

	DEBUG(3,("reply_ntcancel: cancel called on mid = %d.\n", req->mid));

	END_PROFILE(SMBntcancel);
	return;
}