static cycle_t kvm_clock_read(void)
{
	struct pvclock_vcpu_time_info *src;
	cycle_t ret;

	preempt_disable_notrace();
	src = &__get_cpu_var(hv_clock);
	ret = pvclock_clocksource_read(src);
	preempt_enable_notrace();
	return ret;
}