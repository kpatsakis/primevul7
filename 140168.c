int kvm_arch_vcpu_setup(struct kvm_vcpu *vcpu)
{
	int r;

	vcpu->arch.mtrr_state.have_fixed = 1;
	r = vcpu_load(vcpu);
	if (r)
		return r;
	kvm_vcpu_reset(vcpu);
	kvm_mmu_setup(vcpu);
	vcpu_put(vcpu);

	return r;
}