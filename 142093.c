static inline u64 get_kernel_ns(void)
{
	struct timespec ts;

	WARN_ON(preemptible());
	ktime_get_ts(&ts);
	monotonic_to_bootbased(&ts);
	return timespec_to_ns(&ts);
}