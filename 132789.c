static void dummy_recv_done_handler(struct ipmi_recv_msg *msg)
{
	atomic_dec(&panic_done_count);
}