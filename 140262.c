static int handle_halt(struct kvm_vcpu *vcpu)
{
	skip_emulated_instruction(vcpu);
	return kvm_emulate_halt(vcpu);
}