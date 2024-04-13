static bool nested_exit_on_nmi(struct kvm_vcpu *vcpu)
{
	return get_vmcs12(vcpu)->pin_based_vm_exec_control &
		PIN_BASED_NMI_EXITING;
}