static void ept_set_mmio_spte_mask(void)
{
	/*
	 * EPT Misconfigurations can be generated if the value of bits 2:0
	 * of an EPT paging-structure entry is 110b (write/execute).
	 * Also, magic bits (0x3ull << 62) is set to quickly identify mmio
	 * spte.
	 */
	kvm_mmu_set_mmio_spte_mask((0x3ull << 62) | 0x6ull);
}