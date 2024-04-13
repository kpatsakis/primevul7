static inline bool cpu_has_vmx_shadow_vmcs(void)
{
	u64 vmx_msr;
	rdmsrl(MSR_IA32_VMX_MISC, vmx_msr);
	/* check if the cpu supports writing r/o exit information fields */
	if (!(vmx_msr & MSR_IA32_VMX_MISC_VMWRITE_SHADOW_RO_FIELDS))
		return false;

	return vmcs_config.cpu_based_2nd_exec_ctrl &
		SECONDARY_EXEC_SHADOW_VMCS;
}