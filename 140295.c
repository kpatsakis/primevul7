static bool update_transition_efer(struct vcpu_vmx *vmx, int efer_offset)
{
	u64 guest_efer;
	u64 ignore_bits;

	guest_efer = vmx->vcpu.arch.efer;

	/*
	 * NX is emulated; LMA and LME handled by hardware; SCE meaningless
	 * outside long mode
	 */
	ignore_bits = EFER_NX | EFER_SCE;
#ifdef CONFIG_X86_64
	ignore_bits |= EFER_LMA | EFER_LME;
	/* SCE is meaningful only in long mode on Intel */
	if (guest_efer & EFER_LMA)
		ignore_bits &= ~(u64)EFER_SCE;
#endif
	guest_efer &= ~ignore_bits;
	guest_efer |= host_efer & ignore_bits;
	vmx->guest_msrs[efer_offset].data = guest_efer;
	vmx->guest_msrs[efer_offset].mask = ~ignore_bits;

	clear_atomic_switch_msr(vmx, MSR_EFER);
	/* On ept, can't emulate nx, and must switch nx atomically */
	if (enable_ept && ((vmx->vcpu.arch.efer ^ host_efer) & EFER_NX)) {
		guest_efer = vmx->vcpu.arch.efer;
		if (!(guest_efer & EFER_LMA))
			guest_efer &= ~EFER_LME;
		add_atomic_switch_msr(vmx, MSR_EFER, guest_efer, host_efer);
		return false;
	}

	return true;
}