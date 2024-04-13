
static void flush_all_backlogs(void)
{
	unsigned int cpu;

	get_online_cpus();

	for_each_online_cpu(cpu)
		queue_work_on(cpu, system_highpri_wq,
			      per_cpu_ptr(&flush_works, cpu));

	for_each_online_cpu(cpu)
		flush_work(per_cpu_ptr(&flush_works, cpu));

	put_online_cpus();