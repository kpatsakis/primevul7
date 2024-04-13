static unsigned long kvm_get_tsc_khz(void)
{
	struct pvclock_vcpu_time_info *src;
	src = &per_cpu(hv_clock, 0);
	return pvclock_tsc_khz(src);
}