static void vmx_disable_intercept_for_msr(u32 msr, bool longmode_only)
{
	if (!longmode_only)
		__vmx_disable_intercept_for_msr(vmx_msr_bitmap_legacy,
						msr, MSR_TYPE_R | MSR_TYPE_W);
	__vmx_disable_intercept_for_msr(vmx_msr_bitmap_longmode,
						msr, MSR_TYPE_R | MSR_TYPE_W);
}