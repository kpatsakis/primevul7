static int kvm_iommu_unmap_memslots(struct kvm *kvm)
{
	int idx;
	struct kvm_memslots *slots;
	struct kvm_memory_slot *memslot;

	idx = srcu_read_lock(&kvm->srcu);
	slots = kvm_memslots(kvm);

	kvm_for_each_memslot(memslot, slots)
		kvm_iommu_unmap_pages(kvm, memslot);

	srcu_read_unlock(&kvm->srcu, idx);

	if (kvm->arch.iommu_noncoherent)
		kvm_arch_unregister_noncoherent_dma(kvm);

	return 0;
}