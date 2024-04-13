int kvm_arch_create_memslot(struct kvm_memory_slot *slot, unsigned long npages)
{
	int i;

	for (i = 0; i < KVM_NR_PAGE_SIZES; ++i) {
		unsigned long ugfn;
		int lpages;
		int level = i + 1;

		lpages = gfn_to_index(slot->base_gfn + npages - 1,
				      slot->base_gfn, level) + 1;

		slot->arch.rmap[i] =
			kvm_kvzalloc(lpages * sizeof(*slot->arch.rmap[i]));
		if (!slot->arch.rmap[i])
			goto out_free;
		if (i == 0)
			continue;

		slot->arch.lpage_info[i - 1] = kvm_kvzalloc(lpages *
					sizeof(*slot->arch.lpage_info[i - 1]));
		if (!slot->arch.lpage_info[i - 1])
			goto out_free;

		if (slot->base_gfn & (KVM_PAGES_PER_HPAGE(level) - 1))
			slot->arch.lpage_info[i - 1][0].write_count = 1;
		if ((slot->base_gfn + npages) & (KVM_PAGES_PER_HPAGE(level) - 1))
			slot->arch.lpage_info[i - 1][lpages - 1].write_count = 1;
		ugfn = slot->userspace_addr >> PAGE_SHIFT;
		/*
		 * If the gfn and userspace address are not aligned wrt each
		 * other, or if explicitly asked to, disable large page
		 * support for this slot
		 */
		if ((slot->base_gfn ^ ugfn) & (KVM_PAGES_PER_HPAGE(level) - 1) ||
		    !kvm_largepages_enabled()) {
			unsigned long j;

			for (j = 0; j < lpages; ++j)
				slot->arch.lpage_info[i - 1][j].write_count = 1;
		}
	}

	return 0;

out_free:
	for (i = 0; i < KVM_NR_PAGE_SIZES; ++i) {
		kvm_kvfree(slot->arch.rmap[i]);
		slot->arch.rmap[i] = NULL;
		if (i == 0)
			continue;

		kvm_kvfree(slot->arch.lpage_info[i - 1]);
		slot->arch.lpage_info[i - 1] = NULL;
	}
	return -ENOMEM;
}