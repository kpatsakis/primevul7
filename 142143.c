int kvm_arch_vcpu_ioctl_get_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state)
{
	mp_state->mp_state = vcpu->arch.mp_state;
	return 0;
}