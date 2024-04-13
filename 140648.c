static int nested_vmx_check_exception(struct kvm_vcpu *vcpu, unsigned nr)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	if (!(vmcs12->exception_bitmap & (1u << nr)))
		return 0;

	nested_vmx_vmexit(vcpu, to_vmx(vcpu)->exit_reason,
			  vmcs_read32(VM_EXIT_INTR_INFO),
			  vmcs_readl(EXIT_QUALIFICATION));
	return 1;
}