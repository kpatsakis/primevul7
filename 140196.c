static void vmx_hwapic_isr_update(struct kvm *kvm, int isr)
{
	u16 status;
	u8 old;

	if (!vmx_vm_has_apicv(kvm))
		return;

	if (isr == -1)
		isr = 0;

	status = vmcs_read16(GUEST_INTR_STATUS);
	old = status >> 8;
	if (isr != old) {
		status &= 0xff;
		status |= isr << 8;
		vmcs_write16(GUEST_INTR_STATUS, status);
	}
}