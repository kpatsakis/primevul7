BPF_CALL_0(__get_raw_cpu_id)
{
	return raw_smp_processor_id();
}