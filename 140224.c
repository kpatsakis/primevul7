static int kvm_vcpu_ioctl_set_lapic(struct kvm_vcpu *vcpu,
				    struct kvm_lapic_state *s)
{
	kvm_apic_post_state_restore(vcpu, s);
	update_cr8_intercept(vcpu);

	return 0;
}