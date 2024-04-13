static void vmx_sync_pir_to_irr(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!pi_test_and_clear_on(&vmx->pi_desc))
		return;

	kvm_apic_update_irr(vcpu, vmx->pi_desc.pir);
}