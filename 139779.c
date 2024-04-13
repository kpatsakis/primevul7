static inline pte_t pte_mknonnuma(pte_t pte)
{
	pte = pte_clear_flags(pte, _PAGE_NUMA);
	return pte_set_flags(pte, _PAGE_PRESENT|_PAGE_ACCESSED);
}