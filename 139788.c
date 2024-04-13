static inline int pte_numa(pte_t pte)
{
	return (pte_flags(pte) &
		(_PAGE_NUMA|_PAGE_PRESENT)) == _PAGE_NUMA;
}