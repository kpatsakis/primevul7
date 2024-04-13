void kvm_arch_flush_shadow_memslot(struct kvm *kvm,
				   struct kvm_memory_slot *slot)
{
	kvm_mmu_invalidate_zap_all_pages(kvm);
}