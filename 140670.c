int kvm_arch_prepare_memory_region(struct kvm *kvm,
				struct kvm_memory_slot *memslot,
				struct kvm_userspace_memory_region *mem,
				enum kvm_mr_change change)
{
	/*
	 * Only private memory slots need to be mapped here since
	 * KVM_SET_MEMORY_REGION ioctl is no longer supported.
	 */
	if ((memslot->id >= KVM_USER_MEM_SLOTS) && (change == KVM_MR_CREATE)) {
		unsigned long userspace_addr;

		/*
		 * MAP_SHARED to prevent internal slot pages from being moved
		 * by fork()/COW.
		 */
		userspace_addr = vm_mmap(NULL, 0, memslot->npages * PAGE_SIZE,
					 PROT_READ | PROT_WRITE,
					 MAP_SHARED | MAP_ANONYMOUS, 0);

		if (IS_ERR((void *)userspace_addr))
			return PTR_ERR((void *)userspace_addr);

		memslot->userspace_addr = userspace_addr;
	}

	return 0;
}