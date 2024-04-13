static void handle_clts(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu)) {
		/*
		 * We get here when L2 did CLTS, and L1 didn't shadow CR0.TS
		 * but we did (!fpu_active). We need to keep GUEST_CR0.TS on,
		 * just pretend it's off (also in arch.cr0 for fpu_activate).
		 */
		vmcs_writel(CR0_READ_SHADOW,
			vmcs_readl(CR0_READ_SHADOW) & ~X86_CR0_TS);
		vcpu->arch.cr0 &= ~X86_CR0_TS;
	} else
		vmx_set_cr0(vcpu, kvm_read_cr0_bits(vcpu, ~X86_CR0_TS));
}