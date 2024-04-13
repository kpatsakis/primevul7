BPF_CALL_3(bpf_skb_under_cgroup, struct sk_buff *, skb, struct bpf_map *, map,
	   u32, idx)
{
	struct bpf_array *array = container_of(map, struct bpf_array, map);
	struct cgroup *cgrp;
	struct sock *sk;

	sk = skb_to_full_sk(skb);
	if (!sk || !sk_fullsock(sk))
		return -ENOENT;
	if (unlikely(idx >= array->map.max_entries))
		return -E2BIG;

	cgrp = READ_ONCE(array->ptrs[idx]);
	if (unlikely(!cgrp))
		return -EAGAIN;

	return sk_under_cgroup_hierarchy(sk, cgrp);
}