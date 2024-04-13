static void nested_ept_inject_page_fault(struct kvm_vcpu *vcpu,
		struct x86_exception *fault)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	u32 exit_reason;

	if (fault->error_code & PFERR_RSVD_MASK)
		exit_reason = EXIT_REASON_EPT_MISCONFIG;
	else
		exit_reason = EXIT_REASON_EPT_VIOLATION;
	nested_vmx_vmexit(vcpu, exit_reason, 0, vcpu->arch.exit_qualification);
	vmcs12->guest_physical_address = fault->address;
}