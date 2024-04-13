static inline int pmd_numa(pmd_t pmd)
{
	return (pmd_flags(pmd) &
		(_PAGE_NUMA|_PAGE_PRESENT)) == _PAGE_NUMA;
}