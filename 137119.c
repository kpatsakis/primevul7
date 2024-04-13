BPF_CALL_2(bpf_msg_apply_bytes, struct sk_msg_buff *, msg, u32, bytes)
{
	msg->apply_bytes = bytes;
	return 0;
}