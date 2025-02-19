static int dm_request_for_irq_injection(struct kvm_vcpu *vcpu)
{
	return (!irqchip_in_kernel(vcpu->kvm) && !kvm_cpu_has_interrupt(vcpu) &&
		vcpu->run->request_interrupt_window &&
		kvm_arch_interrupt_allowed(vcpu));
}