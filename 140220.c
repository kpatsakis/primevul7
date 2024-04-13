static int kvmclock_cpu_notifier(struct notifier_block *nfb,
					unsigned long action, void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;

	switch (action) {
		case CPU_ONLINE:
		case CPU_DOWN_FAILED:
			smp_call_function_single(cpu, tsc_khz_changed, NULL, 1);
			break;
		case CPU_DOWN_PREPARE:
			smp_call_function_single(cpu, tsc_bad, NULL, 1);
			break;
	}
	return NOTIFY_OK;
}