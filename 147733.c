static int nested_vmx_check_apic_access_controls(struct kvm_vcpu *vcpu,
					  struct vmcs12 *vmcs12)
{
	if (nested_cpu_has2(vmcs12, SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES) &&
	    !page_address_valid(vcpu, vmcs12->apic_access_addr))
		return -EINVAL;
	else
		return 0;
}