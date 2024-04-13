static bool vmx_rdtscp_supported(void)
{
	return cpu_has_vmx_rdtscp();
}