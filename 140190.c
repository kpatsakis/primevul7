static void emulator_write_gpr(struct x86_emulate_ctxt *ctxt, unsigned reg, ulong val)
{
	kvm_register_write(emul_to_vcpu(ctxt), reg, val);
}