static bool vmx_invpcid_supported(void)
{
	return cpu_has_vmx_invpcid() && enable_ept;
}