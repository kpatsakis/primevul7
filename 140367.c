static void __loaded_vmcs_clear(void *arg)
{
	struct loaded_vmcs *loaded_vmcs = arg;
	int cpu = raw_smp_processor_id();

	if (loaded_vmcs->cpu != cpu)
		return; /* vcpu migration can race with cpu offline */
	if (per_cpu(current_vmcs, cpu) == loaded_vmcs->vmcs)
		per_cpu(current_vmcs, cpu) = NULL;
	crash_disable_local_vmclear(cpu);
	list_del(&loaded_vmcs->loaded_vmcss_on_cpu_link);

	/*
	 * we should ensure updating loaded_vmcs->loaded_vmcss_on_cpu_link
	 * is before setting loaded_vmcs->vcpu to -1 which is done in
	 * loaded_vmcs_init. Otherwise, other cpu can see vcpu = -1 fist
	 * then adds the vmcs into percpu list before it is deleted.
	 */
	smp_wmb();

	loaded_vmcs_init(loaded_vmcs);
	crash_enable_local_vmclear(cpu);
}