static void emulator_set_gdt(struct x86_emulate_ctxt *ctxt, struct desc_ptr *dt)
{
	kvm_x86_ops->set_gdt(emul_to_vcpu(ctxt), dt);
}