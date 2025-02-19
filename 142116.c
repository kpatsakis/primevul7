int __kvm_set_xcr(struct kvm_vcpu *vcpu, u32 index, u64 xcr)
{
	u64 xcr0;

	/* Only support XCR_XFEATURE_ENABLED_MASK(xcr0) now  */
	if (index != XCR_XFEATURE_ENABLED_MASK)
		return 1;
	xcr0 = xcr;
	if (kvm_x86_ops->get_cpl(vcpu) != 0)
		return 1;
	if (!(xcr0 & XSTATE_FP))
		return 1;
	if ((xcr0 & XSTATE_YMM) && !(xcr0 & XSTATE_SSE))
		return 1;
	if (xcr0 & ~host_xcr0)
		return 1;
	vcpu->arch.xcr0 = xcr0;
	vcpu->guest_xcr0_loaded = 0;
	return 0;
}