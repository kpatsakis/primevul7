static bool need_emulate_wbinvd(struct kvm_vcpu *vcpu)
{
	return vcpu->kvm->arch.iommu_domain &&
		!(vcpu->kvm->arch.iommu_flags & KVM_IOMMU_CACHE_COHERENCY);
}