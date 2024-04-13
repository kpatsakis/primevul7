static int __replace_page(struct vm_area_struct *vma, unsigned long addr,
				struct page *old_page, struct page *new_page)
{
	struct mm_struct *mm = vma->vm_mm;
	struct page_vma_mapped_walk pvmw = {
		.page = old_page,
		.vma = vma,
		.address = addr,
	};
	int err;
	/* For mmu_notifiers */
	const unsigned long mmun_start = addr;
	const unsigned long mmun_end   = addr + PAGE_SIZE;
	struct mem_cgroup *memcg;

	VM_BUG_ON_PAGE(PageTransHuge(old_page), old_page);

	err = mem_cgroup_try_charge(new_page, vma->vm_mm, GFP_KERNEL, &memcg,
			false);
	if (err)
		return err;

	/* For try_to_free_swap() and munlock_vma_page() below */
	lock_page(old_page);

	mmu_notifier_invalidate_range_start(mm, mmun_start, mmun_end);
	err = -EAGAIN;
	if (!page_vma_mapped_walk(&pvmw)) {
		mem_cgroup_cancel_charge(new_page, memcg, false);
		goto unlock;
	}
	VM_BUG_ON_PAGE(addr != pvmw.address, old_page);

	get_page(new_page);
	page_add_new_anon_rmap(new_page, vma, addr, false);
	mem_cgroup_commit_charge(new_page, memcg, false, false);
	lru_cache_add_active_or_unevictable(new_page, vma);

	if (!PageAnon(old_page)) {
		dec_mm_counter(mm, mm_counter_file(old_page));
		inc_mm_counter(mm, MM_ANONPAGES);
	}

	flush_cache_page(vma, addr, pte_pfn(*pvmw.pte));
	ptep_clear_flush_notify(vma, addr, pvmw.pte);
	set_pte_at_notify(mm, addr, pvmw.pte,
			mk_pte(new_page, vma->vm_page_prot));

	page_remove_rmap(old_page, false);
	if (!page_mapped(old_page))
		try_to_free_swap(old_page);
	page_vma_mapped_walk_done(&pvmw);

	if (vma->vm_flags & VM_LOCKED)
		munlock_vma_page(old_page);
	put_page(old_page);

	err = 0;
 unlock:
	mmu_notifier_invalidate_range_end(mm, mmun_start, mmun_end);
	unlock_page(old_page);
	return err;
}