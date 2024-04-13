static void kvm_iommu_put_pages(struct kvm *kvm,
				gfn_t base_gfn, unsigned long npages)
{
	struct iommu_domain *domain;
	gfn_t end_gfn, gfn;
	pfn_t pfn;
	u64 phys;

	domain  = kvm->arch.iommu_domain;
	end_gfn = base_gfn + npages;
	gfn     = base_gfn;

	/* check if iommu exists and in use */
	if (!domain)
		return;

	while (gfn < end_gfn) {
		unsigned long unmap_pages;
		size_t size;

		/* Get physical address */
		phys = iommu_iova_to_phys(domain, gfn_to_gpa(gfn));

		if (!phys) {
			gfn++;
			continue;
		}

		pfn  = phys >> PAGE_SHIFT;

		/* Unmap address from IO address space */
		size       = iommu_unmap(domain, gfn_to_gpa(gfn), PAGE_SIZE);
		unmap_pages = 1ULL << get_order(size);

		/* Unpin all pages we just unmapped to not leak any memory */
		kvm_unpin_pages(kvm, pfn, unmap_pages);

		gfn += unmap_pages;
	}
}