static void dummy_smi_done_handler(struct ipmi_smi_msg *msg)
{
	atomic_dec(&panic_done_count);
}