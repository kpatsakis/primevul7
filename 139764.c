static inline pmd_t pmd_mknuma(pmd_t pmd)
{
	pmd = pmd_set_flags(pmd, _PAGE_NUMA);
	return pmd_clear_flags(pmd, _PAGE_PRESENT);
}