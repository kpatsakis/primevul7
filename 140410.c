static inline bool cpu_has_vmx_posted_intr(void)
{
	return vmcs_config.pin_based_exec_ctrl & PIN_BASED_POSTED_INTR;
}