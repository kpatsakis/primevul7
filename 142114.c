static int kvm_vcpu_ioctl_interrupt(struct kvm_vcpu *vcpu,
				    struct kvm_interrupt *irq)
{
	if (irq->irq < 0 || irq->irq >= KVM_NR_INTERRUPTS)
		return -EINVAL;
	if (irqchip_in_kernel(vcpu->kvm))
		return -ENXIO;

	kvm_queue_interrupt(vcpu, irq->irq, false);
	kvm_make_request(KVM_REQ_EVENT, vcpu);

	return 0;
}