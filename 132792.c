static int deliver_response(struct ipmi_smi *intf, struct ipmi_recv_msg *msg)
{
	int rv = 0;

	if (!msg->user) {
		/* Special handling for NULL users. */
		if (intf->null_user_handler) {
			intf->null_user_handler(intf, msg);
		} else {
			/* No handler, so give up. */
			rv = -EINVAL;
		}
		ipmi_free_recv_msg(msg);
	} else if (oops_in_progress) {
		/*
		 * If we are running in the panic context, calling the
		 * receive handler doesn't much meaning and has a deadlock
		 * risk.  At this moment, simply skip it in that case.
		 */
		ipmi_free_recv_msg(msg);
	} else {
		int index;
		struct ipmi_user *user = acquire_ipmi_user(msg->user, &index);

		if (user) {
			user->handler->ipmi_recv_hndl(msg, user->handler_data);
			release_ipmi_user(user, index);
		} else {
			/* User went away, give up. */
			ipmi_free_recv_msg(msg);
			rv = -EINVAL;
		}
	}

	return rv;
}