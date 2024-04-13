static bool kvm_get_time_and_clockread(s64 *kernel_ns, cycle_t *cycle_now)
{
	/* checked again under seqlock below */
	if (pvclock_gtod_data.clock.vclock_mode != VCLOCK_TSC)
		return false;

	return do_monotonic_boot(kernel_ns, cycle_now) == VCLOCK_TSC;
}