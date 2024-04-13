static void vmx_set_tsc_khz(struct kvm_vcpu *vcpu, u32 user_tsc_khz, bool scale)
{
	if (!scale)
		return;

	if (user_tsc_khz > tsc_khz) {
		vcpu->arch.tsc_catchup = 1;
		vcpu->arch.tsc_always_catchup = 1;
	} else
		WARN(1, "user requested TSC rate below hardware speed\n");
}