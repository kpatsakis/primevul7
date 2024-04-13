static void tsc_bad(void *info)
{
	__this_cpu_write(cpu_tsc_khz, 0);
}