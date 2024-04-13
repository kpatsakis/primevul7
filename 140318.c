static int handle_nop(struct kvm_vcpu *vcpu)
{
	skip_emulated_instruction(vcpu);
	return 1;
}