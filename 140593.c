static void fx_free(struct kvm_vcpu *vcpu)
{
	fpu_free(&vcpu->arch.guest_fpu);
}