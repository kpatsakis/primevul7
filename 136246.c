int kvm_iommu_unmap_guest(struct kvm *kvm)
{
	struct iommu_domain *domain = kvm->arch.iommu_domain;

	/* check if iommu exists and in use */
	if (!domain)
		return 0;

	mutex_lock(&kvm->slots_lock);
	kvm_iommu_unmap_memslots(kvm);
	kvm->arch.iommu_domain = NULL;
	kvm->arch.iommu_noncoherent = false;
	mutex_unlock(&kvm->slots_lock);

	iommu_domain_free(domain);
	return 0;
}