static inline pmd_t pmd_mknonnuma(pmd_t pmd)
{
	pmd = pmd_clear_flags(pmd, _PAGE_NUMA);
	return pmd_set_flags(pmd, _PAGE_PRESENT|_PAGE_ACCESSED);
}