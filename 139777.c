static inline pte_t pte_mknuma(pte_t pte)
{
	pte = pte_set_flags(pte, _PAGE_NUMA);
	return pte_clear_flags(pte, _PAGE_PRESENT);
}