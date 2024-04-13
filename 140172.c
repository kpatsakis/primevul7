static void vmx_enable_intercept_msr_read_x2apic(u32 msr)
{
	__vmx_enable_intercept_for_msr(vmx_msr_bitmap_legacy_x2apic,
			msr, MSR_TYPE_R);
	__vmx_enable_intercept_for_msr(vmx_msr_bitmap_longmode_x2apic,
			msr, MSR_TYPE_R);
}