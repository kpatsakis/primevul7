static void vmx_flush_tlb(struct kvm_vcpu *vcpu)
{
	vpid_sync_context(to_vmx(vcpu));
	if (enable_ept) {
		if (!VALID_PAGE(vcpu->arch.mmu.root_hpa))
			return;
		ept_sync_context(construct_eptp(vcpu->arch.mmu.root_hpa));
	}
}