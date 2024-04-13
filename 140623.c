static u64 vmx_read_tsc_offset(struct kvm_vcpu *vcpu)
{
	return vmcs_read64(TSC_OFFSET);
}