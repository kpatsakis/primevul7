static int alloc_identity_pagetable(struct kvm *kvm)
{
	/* Called with kvm->slots_lock held. */

	struct kvm_userspace_memory_region kvm_userspace_mem;
	int r = 0;

	BUG_ON(kvm->arch.ept_identity_pagetable_done);

	kvm_userspace_mem.slot = IDENTITY_PAGETABLE_PRIVATE_MEMSLOT;
	kvm_userspace_mem.flags = 0;
	kvm_userspace_mem.guest_phys_addr =
		kvm->arch.ept_identity_map_addr;
	kvm_userspace_mem.memory_size = PAGE_SIZE;
	r = __kvm_set_memory_region(kvm, &kvm_userspace_mem);

	return r;
}