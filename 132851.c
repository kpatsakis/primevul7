static void __cpuinit kvm_setup_secondary_clock(void)
{
	/*
	 * Now that the first cpu already had this clocksource initialized,
	 * we shouldn't fail.
	 */
	WARN_ON(kvm_register_clock("secondary cpu clock"));
	/* ok, done with our trickery, call native */
	setup_secondary_APIC_clock();
}