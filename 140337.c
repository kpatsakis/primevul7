static int emulator_get_msr(struct x86_emulate_ctxt *ctxt,
			    u32 msr_index, u64 *pdata)
{
	return kvm_get_msr(emul_to_vcpu(ctxt), msr_index, pdata);
}