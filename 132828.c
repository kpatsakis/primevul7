static void ipmi_panic_request_and_wait(struct ipmi_smi *intf,
					struct ipmi_addr *addr,
					struct kernel_ipmi_msg *msg)
{
	struct ipmi_smi_msg  smi_msg;
	struct ipmi_recv_msg recv_msg;
	int rv;

	smi_msg.done = dummy_smi_done_handler;
	recv_msg.done = dummy_recv_done_handler;
	atomic_add(2, &panic_done_count);
	rv = i_ipmi_request(NULL,
			    intf,
			    addr,
			    0,
			    msg,
			    intf,
			    &smi_msg,
			    &recv_msg,
			    0,
			    intf->addrinfo[0].address,
			    intf->addrinfo[0].lun,
			    0, 1); /* Don't retry, and don't wait. */
	if (rv)
		atomic_sub(2, &panic_done_count);
	else if (intf->handlers->flush_messages)
		intf->handlers->flush_messages(intf->send_info);

	while (atomic_read(&panic_done_count) != 0)
		ipmi_poll(intf);
}