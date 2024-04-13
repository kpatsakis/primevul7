static u64 vmx_get_dr6(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.dr6;
}