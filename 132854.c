static unsigned long kvm_get_wallclock(void)
{
	struct pvclock_vcpu_time_info *vcpu_time;
	struct timespec ts;
	int low, high;

	low = (int)__pa_symbol(&wall_clock);
	high = ((u64)__pa_symbol(&wall_clock) >> 32);

	native_write_msr(msr_kvm_wall_clock, low, high);

	vcpu_time = &get_cpu_var(hv_clock);
	pvclock_read_wallclock(&wall_clock, vcpu_time, &ts);
	put_cpu_var(hv_clock);

	return ts.tv_sec;
}