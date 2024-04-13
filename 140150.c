bool kvm_is_linear_rip(struct kvm_vcpu *vcpu, unsigned long linear_rip)
{
	unsigned long current_rip = kvm_rip_read(vcpu) +
		get_segment_base(vcpu, VCPU_SREG_CS);

	return current_rip == linear_rip;
}