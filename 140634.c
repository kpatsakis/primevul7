static inline u32 kvm_async_pf_hash_fn(gfn_t gfn)
{
	return hash_32(gfn & 0xffffffff, order_base_2(ASYNC_PF_PER_VCPU));
}