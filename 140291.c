static void kvm_unload_vcpu_mmu(struct kvm_vcpu *vcpu)
{
	int r;
	r = vcpu_load(vcpu);
	BUG_ON(r);
	kvm_mmu_unload(vcpu);
	vcpu_put(vcpu);
}