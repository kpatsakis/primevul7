int kvm_register_clock(char *txt)
{
	int cpu = smp_processor_id();
	int low, high, ret;

	low = (int)__pa(&per_cpu(hv_clock, cpu)) | 1;
	high = ((u64)__pa(&per_cpu(hv_clock, cpu)) >> 32);
	ret = native_write_msr_safe(msr_kvm_system_time, low, high);
	printk(KERN_INFO "kvm-clock: cpu %d, msr %x:%x, %s\n",
	       cpu, high, low, txt);

	return ret;
}