static int handle_apic_eoi_induced(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	int vector = exit_qualification & 0xff;

	/* EOI-induced VM exit is trap-like and thus no need to adjust IP */
	kvm_apic_set_eoi_accelerated(vcpu, vector);
	return 1;
}