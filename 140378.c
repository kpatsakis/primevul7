static int emulator_get_cpl(struct x86_emulate_ctxt *ctxt)
{
	return kvm_x86_ops->get_cpl(emul_to_vcpu(ctxt));
}