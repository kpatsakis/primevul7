static void vmx_sched_in(struct kvm_vcpu *vcpu, int cpu)
{
	if (ple_gap)
		shrink_ple_window(vcpu);
}