int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm, struct kvm_dirty_log *log)
{
	int r;
	struct kvm_memory_slot *memslot;
	unsigned long n, i;
	unsigned long *dirty_bitmap;
	unsigned long *dirty_bitmap_buffer;
	bool is_dirty = false;

	mutex_lock(&kvm->slots_lock);

	r = -EINVAL;
	if (log->slot >= KVM_USER_MEM_SLOTS)
		goto out;

	memslot = id_to_memslot(kvm->memslots, log->slot);

	dirty_bitmap = memslot->dirty_bitmap;
	r = -ENOENT;
	if (!dirty_bitmap)
		goto out;

	n = kvm_dirty_bitmap_bytes(memslot);

	dirty_bitmap_buffer = dirty_bitmap + n / sizeof(long);
	memset(dirty_bitmap_buffer, 0, n);

	spin_lock(&kvm->mmu_lock);

	for (i = 0; i < n / sizeof(long); i++) {
		unsigned long mask;
		gfn_t offset;

		if (!dirty_bitmap[i])
			continue;

		is_dirty = true;

		mask = xchg(&dirty_bitmap[i], 0);
		dirty_bitmap_buffer[i] = mask;

		offset = i * BITS_PER_LONG;
		kvm_mmu_write_protect_pt_masked(kvm, memslot, offset, mask);
	}

	spin_unlock(&kvm->mmu_lock);

	/* See the comments in kvm_mmu_slot_remove_write_access(). */
	lockdep_assert_held(&kvm->slots_lock);

	/*
	 * All the TLBs can be flushed out of mmu lock, see the comments in
	 * kvm_mmu_slot_remove_write_access().
	 */
	if (is_dirty)
		kvm_flush_remote_tlbs(kvm);

	r = -EFAULT;
	if (copy_to_user(log->dirty_bitmap, dirty_bitmap_buffer, n))
		goto out;

	r = 0;
out:
	mutex_unlock(&kvm->slots_lock);
	return r;
}