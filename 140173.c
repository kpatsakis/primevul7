static int emulator_fix_hypercall(struct x86_emulate_ctxt *ctxt)
{
	struct kvm_vcpu *vcpu = emul_to_vcpu(ctxt);
	char instruction[3];
	unsigned long rip = kvm_rip_read(vcpu);

	kvm_x86_ops->patch_hypercall(vcpu, instruction);

	return emulator_write_emulated(ctxt, rip, instruction, 3, NULL);
}