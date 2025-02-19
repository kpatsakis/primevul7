static int kvm_vcpu_ioctl_get_lapic(struct kvm_vcpu *vcpu,
				    struct kvm_lapic_state *s)
{
	kvm_x86_ops->sync_pir_to_irr(vcpu);
	memcpy(s->regs, vcpu->arch.apic->regs, sizeof *s);

	return 0;
}