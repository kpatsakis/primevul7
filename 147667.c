static bool nested_cr3_valid(struct kvm_vcpu *vcpu, unsigned long val)
{
	unsigned long invalid_mask;

	invalid_mask = (~0ULL) << cpuid_maxphyaddr(vcpu);
	return (val & invalid_mask) == 0;
}