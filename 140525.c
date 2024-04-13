static bool vmx_get_nmi_mask(struct kvm_vcpu *vcpu)
{
	if (!cpu_has_virtual_nmis())
		return to_vmx(vcpu)->soft_vnmi_blocked;
	if (to_vmx(vcpu)->nmi_known_unmasked)
		return false;
	return vmcs_read32(GUEST_INTERRUPTIBILITY_INFO)	& GUEST_INTR_STATE_NMI;
}