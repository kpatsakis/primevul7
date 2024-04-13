static inline u64 get_kernel_ns(void)
{
	return ktime_get_boot_ns();
}