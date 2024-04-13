static void vmx_deliver_posted_interrupt(struct kvm_vcpu *vcpu, int vector)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int r;

	if (pi_test_and_set_pir(vector, &vmx->pi_desc))
		return;

	r = pi_test_and_set_on(&vmx->pi_desc);
	kvm_make_request(KVM_REQ_EVENT, vcpu);
#ifdef CONFIG_SMP
	if (!r && (vcpu->mode == IN_GUEST_MODE))
		apic->send_IPI_mask(get_cpu_mask(vcpu->cpu),
				POSTED_INTR_VECTOR);
	else
#endif
		kvm_vcpu_kick(vcpu);
}