void __init kvmclock_init(void)
{
	if (!kvm_para_available())
		return;

	if (kvmclock && kvm_para_has_feature(KVM_FEATURE_CLOCKSOURCE2)) {
		msr_kvm_system_time = MSR_KVM_SYSTEM_TIME_NEW;
		msr_kvm_wall_clock = MSR_KVM_WALL_CLOCK_NEW;
	} else if (!(kvmclock && kvm_para_has_feature(KVM_FEATURE_CLOCKSOURCE)))
		return;

	printk(KERN_INFO "kvm-clock: Using msrs %x and %x",
		msr_kvm_system_time, msr_kvm_wall_clock);

	if (kvm_register_clock("boot clock"))
		return;
	pv_time_ops.sched_clock = kvm_clock_read;
	x86_platform.calibrate_tsc = kvm_get_tsc_khz;
	x86_platform.get_wallclock = kvm_get_wallclock;
	x86_platform.set_wallclock = kvm_set_wallclock;
#ifdef CONFIG_X86_LOCAL_APIC
	x86_cpuinit.setup_percpu_clockev =
		kvm_setup_secondary_clock;
#endif
	machine_ops.shutdown  = kvm_shutdown;
#ifdef CONFIG_KEXEC
	machine_ops.crash_shutdown  = kvm_crash_shutdown;
#endif
	kvm_get_preset_lpj();
	clocksource_register_hz(&kvm_clock, NSEC_PER_SEC);
	pv_info.paravirt_enabled = 1;
	pv_info.name = "KVM";

	if (kvm_para_has_feature(KVM_FEATURE_CLOCKSOURCE_STABLE_BIT))
		pvclock_set_flags(PVCLOCK_TSC_STABLE_BIT);
}