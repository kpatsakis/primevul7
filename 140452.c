void kvm_track_tsc_matching(struct kvm_vcpu *vcpu)
{
#ifdef CONFIG_X86_64
	bool vcpus_matched;
	bool do_request = false;
	struct kvm_arch *ka = &vcpu->kvm->arch;
	struct pvclock_gtod_data *gtod = &pvclock_gtod_data;

	vcpus_matched = (ka->nr_vcpus_matched_tsc + 1 ==
			 atomic_read(&vcpu->kvm->online_vcpus));

	if (vcpus_matched && gtod->clock.vclock_mode == VCLOCK_TSC)
		if (!ka->use_master_clock)
			do_request = 1;

	if (!vcpus_matched && ka->use_master_clock)
			do_request = 1;

	if (do_request)
		kvm_make_request(KVM_REQ_MASTERCLOCK_UPDATE, vcpu);

	trace_kvm_track_tsc(vcpu->vcpu_id, ka->nr_vcpus_matched_tsc,
			    atomic_read(&vcpu->kvm->online_vcpus),
		            ka->use_master_clock, gtod->clock.vclock_mode);
#endif
}