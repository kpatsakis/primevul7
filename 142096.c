void kvm_set_rflags(struct kvm_vcpu *vcpu, unsigned long rflags)
{
	if (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP &&
	    kvm_is_linear_rip(vcpu, vcpu->arch.singlestep_rip))
		rflags |= X86_EFLAGS_TF;
	kvm_x86_ops->set_rflags(vcpu, rflags);
	kvm_make_request(KVM_REQ_EVENT, vcpu);
}