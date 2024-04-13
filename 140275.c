static u64 vmx_get_mt_mask(struct kvm_vcpu *vcpu, gfn_t gfn, bool is_mmio)
{
	u64 ret;

	/* For VT-d and EPT combination
	 * 1. MMIO: always map as UC
	 * 2. EPT with VT-d:
	 *   a. VT-d without snooping control feature: can't guarantee the
	 *	result, try to trust guest.
	 *   b. VT-d with snooping control feature: snooping control feature of
	 *	VT-d engine can guarantee the cache correctness. Just set it
	 *	to WB to keep consistent with host. So the same as item 3.
	 * 3. EPT without VT-d: always map as WB and set IPAT=1 to keep
	 *    consistent with host MTRR
	 */
	if (is_mmio)
		ret = MTRR_TYPE_UNCACHABLE << VMX_EPT_MT_EPTE_SHIFT;
	else if (kvm_arch_has_noncoherent_dma(vcpu->kvm))
		ret = kvm_get_guest_memory_type(vcpu, gfn) <<
		      VMX_EPT_MT_EPTE_SHIFT;
	else
		ret = (MTRR_TYPE_WRBACK << VMX_EPT_MT_EPTE_SHIFT)
			| VMX_EPT_IPAT_BIT;

	return ret;
}