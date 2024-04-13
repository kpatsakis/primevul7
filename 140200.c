static bool kvm_hv_hypercall_enabled(struct kvm *kvm)
{
	return kvm->arch.hv_hypercall & HV_X64_MSR_HYPERCALL_ENABLE;
}