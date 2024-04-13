static void emulator_set_idt(struct x86_emulate_ctxt *ctxt, struct desc_ptr *dt)
{
	kvm_x86_ops->set_idt(emul_to_vcpu(ctxt), dt);
}