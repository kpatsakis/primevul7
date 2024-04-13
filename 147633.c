static int nested_vmx_check_nmi_controls(struct vmcs12 *vmcs12)
{
	if (!nested_cpu_has_nmi_exiting(vmcs12) &&
	    nested_cpu_has_virtual_nmis(vmcs12))
		return -EINVAL;

	if (!nested_cpu_has_virtual_nmis(vmcs12) &&
	    nested_cpu_has(vmcs12, CPU_BASED_VIRTUAL_NMI_PENDING))
		return -EINVAL;

	return 0;
}