static int emulator_read_pmc(struct x86_emulate_ctxt *ctxt,
			     u32 pmc, u64 *pdata)
{
	return kvm_pmu_read_pmc(emul_to_vcpu(ctxt), pmc, pdata);
}