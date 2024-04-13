static int init_rmode_identity_map(struct kvm *kvm)
{
	int i, idx, r = 0;
	pfn_t identity_map_pfn;
	u32 tmp;

	if (!enable_ept)
		return 0;

	/* Protect kvm->arch.ept_identity_pagetable_done. */
	mutex_lock(&kvm->slots_lock);

	if (likely(kvm->arch.ept_identity_pagetable_done))
		goto out2;

	identity_map_pfn = kvm->arch.ept_identity_map_addr >> PAGE_SHIFT;

	r = alloc_identity_pagetable(kvm);
	if (r < 0)
		goto out2;

	idx = srcu_read_lock(&kvm->srcu);
	r = kvm_clear_guest_page(kvm, identity_map_pfn, 0, PAGE_SIZE);
	if (r < 0)
		goto out;
	/* Set up identity-mapping pagetable for EPT in real mode */
	for (i = 0; i < PT32_ENT_PER_PAGE; i++) {
		tmp = (i << 22) + (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER |
			_PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_PSE);
		r = kvm_write_guest_page(kvm, identity_map_pfn,
				&tmp, i * sizeof(tmp), sizeof(tmp));
		if (r < 0)
			goto out;
	}
	kvm->arch.ept_identity_pagetable_done = true;

out:
	srcu_read_unlock(&kvm->srcu, idx);

out2:
	mutex_unlock(&kvm->slots_lock);
	return r;
}