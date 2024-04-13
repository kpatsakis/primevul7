BPF_CALL_1(bpf_get_cgroup_classid, const struct sk_buff *, skb)
{
	return task_get_classid(skb);
}