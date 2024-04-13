static inline void kvm_async_pf_hash_reset(struct kvm_vcpu *vcpu)
{
	int i;
	for (i = 0; i < roundup_pow_of_two(ASYNC_PF_PER_VCPU); i++)
		vcpu->arch.apf.gfns[i] = ~0;
}