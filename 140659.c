static int vmx_set_tss_addr(struct kvm *kvm, unsigned int addr)
{
	int ret;
	struct kvm_userspace_memory_region tss_mem = {
		.slot = TSS_PRIVATE_MEMSLOT,
		.guest_phys_addr = addr,
		.memory_size = PAGE_SIZE * 3,
		.flags = 0,
	};

	ret = kvm_set_memory_region(kvm, &tss_mem);
	if (ret)
		return ret;
	kvm->arch.tss_addr = addr;
	return init_rmode_tss(kvm);
}