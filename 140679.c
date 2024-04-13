static void vmx_leave_nested(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu))
		nested_vmx_vmexit(vcpu, -1, 0, 0);
	free_nested(to_vmx(vcpu));
}