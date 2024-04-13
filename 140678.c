static void kvm_shared_msr_cpu_online(void)
{
	unsigned i;

	for (i = 0; i < shared_msrs_global.nr; ++i)
		shared_msr_update(i, shared_msrs_global.msrs[i]);
}