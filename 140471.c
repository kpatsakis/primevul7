static int vmx_interrupt_allowed(struct kvm_vcpu *vcpu)
{
	return (!to_vmx(vcpu)->nested.nested_run_pending &&
		vmcs_readl(GUEST_RFLAGS) & X86_EFLAGS_IF) &&
		!(vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
			(GUEST_INTR_STATE_STI | GUEST_INTR_STATE_MOV_SS));
}