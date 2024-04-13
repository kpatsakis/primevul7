static void emulator_set_rflags(struct x86_emulate_ctxt *ctxt, ulong val)
{
	kvm_set_rflags(emul_to_vcpu(ctxt), val);
}