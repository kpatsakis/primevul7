int fx_init(struct kvm_vcpu *vcpu)
{
	int err;

	err = fpu_alloc(&vcpu->arch.guest_fpu);
	if (err)
		return err;

	fpu_finit(&vcpu->arch.guest_fpu);

	/*
	 * Ensure guest xcr0 is valid for loading
	 */
	vcpu->arch.xcr0 = XSTATE_FP;

	vcpu->arch.cr0 |= X86_CR0_ET;

	return 0;
}