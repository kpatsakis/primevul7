static void emulator_get_cpuid(struct x86_emulate_ctxt *ctxt,
			       u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	kvm_cpuid(emul_to_vcpu(ctxt), eax, ebx, ecx, edx);
}