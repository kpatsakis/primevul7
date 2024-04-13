static void vmx_fpu_deactivate(struct kvm_vcpu *vcpu)
{
	/* Note that there is no vcpu->fpu_active = 0 here. The caller must
	 * set this *before* calling this function.
	 */
	vmx_decache_cr0_guest_bits(vcpu);
	vmcs_set_bits(GUEST_CR0, X86_CR0_TS | X86_CR0_MP);
	update_exception_bitmap(vcpu);
	vcpu->arch.cr0_guest_owned_bits = 0;
	vmcs_writel(CR0_GUEST_HOST_MASK, ~vcpu->arch.cr0_guest_owned_bits);
	if (is_guest_mode(vcpu)) {
		/*
		 * L1's specified read shadow might not contain the TS bit,
		 * so now that we turned on shadowing of this bit, we need to
		 * set this bit of the shadow. Like in nested_vmx_run we need
		 * nested_read_cr0(vmcs12), but vmcs12->guest_cr0 is not yet
		 * up-to-date here because we just decached cr0.TS (and we'll
		 * only update vmcs12->guest_cr0 on nested exit).
		 */
		struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
		vmcs12->guest_cr0 = (vmcs12->guest_cr0 & ~X86_CR0_TS) |
			(vcpu->arch.cr0 & X86_CR0_TS);
		vmcs_writel(CR0_READ_SHADOW, nested_read_cr0(vmcs12));
	} else
		vmcs_writel(CR0_READ_SHADOW, vcpu->arch.cr0);
}