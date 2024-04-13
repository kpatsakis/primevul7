static bool nested_cr0_valid(struct vmcs12 *vmcs12, unsigned long val)
{
	unsigned long always_on = VMXON_CR0_ALWAYSON;

	if (nested_vmx_secondary_ctls_high &
		SECONDARY_EXEC_UNRESTRICTED_GUEST &&
	    nested_cpu_has2(vmcs12, SECONDARY_EXEC_UNRESTRICTED_GUEST))
		always_on &= ~(X86_CR0_PE | X86_CR0_PG);
	return (val & always_on) == always_on;
}