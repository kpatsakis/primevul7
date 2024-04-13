static bool nested_exit_on_nmi(struct kvm_vcpu *vcpu)
{
	return nested_cpu_has_nmi_exiting(get_vmcs12(vcpu));
}