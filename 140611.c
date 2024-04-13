void kvm_arch_flush_shadow_all(struct kvm *kvm)
{
	kvm_mmu_invalidate_zap_all_pages(kvm);
}