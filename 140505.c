static inline u32 kvm_async_pf_next_probe(u32 key)
{
	return (key + 1) & (roundup_pow_of_two(ASYNC_PF_PER_VCPU) - 1);
}