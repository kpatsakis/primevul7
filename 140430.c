bool kvm_arch_can_inject_async_page_present(struct kvm_vcpu *vcpu)
{
	if (!(vcpu->arch.apf.msr_val & KVM_ASYNC_PF_ENABLED))
		return true;
	else
		return !kvm_event_needs_reinjection(vcpu) &&
			kvm_x86_ops->interrupt_allowed(vcpu);
}